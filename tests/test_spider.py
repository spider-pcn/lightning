# A lot of this code has been helped by cdecker
from fixtures import *
import time
import os

plugin_path = os.path.join(os.path.dirname(__file__), "plugins", "spider_routing.py")

""" make sure regular payment with channel balances goes through 
"""
def test_regressive(node_factory, executor):
    """Line graph with the middle node (l2) running the plugin.
    """
    l1, l2, l3 = node_factory.line_graph(
        3,  # We want 3 nodes
        opts=[{}, {'plugin': plugin_path}, {}],  # Start l2 with plugin
        wait_for_announce=True  # Let nodes finish gossip before returning
    )

    inv = l3.rpc.invoice(42, "lbl{:}".format(int(time.time())), "description")['bolt11']

    # Let the pay run in the background (on an executor thread) so we don't
    # wait for the pay to succeed before we can check in with the plugin.
    f = executor.submit(l1.rpc.pay, inv)

    # Now see that the plugin queues it
    l2.daemon.wait_for_log(r'Queueing HTLC')

    # Now retrieve the result from the `pay` task we passed to the executor
    # above. If it failed the exception would get re-raised and fail this
    # test, so just retrieving is enough to check it went through
    f.result()


""" ensure that node's incoming link has some spendable msat to make route is found
"""
def check_spendable(node):
    peer = node.rpc.listpeers().get('peers')[0]
    spendable_msat = int(peer['channels'][0]['spendable_msatoshi'])
    print("spendable msat", spendable_msat)
    while spendable_msat == 0:
        time.sleep(15)
        peer = node.rpc.listpeers().get('peers')[0]
        spendable_msat = int(peer['channels'][0]['spendable_msatoshi'])
        print("spendable msat", spendable_msat)


"""  when there isn't insufficient balance, payment gets queued and completed later
"""
def test_payment_completion(node_factory, executor):
    l1, l2, l3 = node_factory.line_graph(
        3,  # We want 3 nodes
        opts=[{}, {'plugin': plugin_path}, {}],  # Start l2 with plugin
        wait_for_announce=True  # Let nodes finish gossip before returning
    )

    completed = []
    futures = []

    # send a single payment for reserve amount first so that future payments
    # can actually get queued and completed
    # otherwise no payments will succeed until reserve amount has been accumulated on l3's end
    inv = l3.rpc.invoice(10000000, "buffer payment{:}".format(int(time.time())), "description")['bolt11']
    f = executor.submit(l1.rpc.pay, inv)
    f.result()

    def trypay(i, sender, receiver):
        # Small helper initiating a payment and then inserting itself into the
        # list of completed order in the order of completion.
        inv = receiver.rpc.invoice(10, "lbl{:}".format(i), "description")['bolt11']
        sender.rpc.pay(inv)
        completed.append(i)

    # Queue all payment attempts, and remember the futures.
    for i in range(10):
        sender = l1 if i % 2 == 0 else l3
        receiver = l3 if i % 2 == 0 else l1
        futures.append(executor.submit(trypay, i, sender, receiver))

    # ensure that that spendable msats > 0 before attempting the payments 
    # tests fail if not for this check
    check_spendable(l2)

    # Now wait for all futures to complete.
    for i, f in enumerate(futures):
        check_spendable(l2)
        print("Future number", i, completed)
        f.result()
    
    # Now check that all of them completed, should be FIFO
    print(completed)
    assert(len(completed) == 10)
