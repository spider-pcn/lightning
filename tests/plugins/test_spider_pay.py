# A lot of this code has been helped by cdecker

from pyln.testing.fixtures import *
from time import time

plugin_path = os.path.join(os.path.dirname(__file__), "spider_pay.py")

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

    inv = l3.rpc.invoice(42, "lbl{:}".format(int(time())), "description")['bolt11']

    # Let the pay run in the background (on an executor thread) so we don't
    # wait for the pay to succeed before we can check in with the plugin.
    f = executor.submit(l1.rpc.spider_pay, inv)
    # Now see that the plugin queues it
    l3.daemon.wait_for_log(r'attempting to send payment')
    l3.daemon.wait_for_log(r'sendpay_success recored')

    # Now retrieve the result from the `pay` task we passed to the executor
    # above. If it failed the exception would get re-raised and fail this
    # test, so just retrieving is enough to check it went through
    f.result()


# """  when there isn't insufficient balance, payment gets queued and completed later
# """
# def test_payment_completion(node_factory, executor):
#     l1, l2, l3 = node_factory.line_graph(
#         3,  # We want 3 nodes
#         opts=[{}, {'plugin': plugin_path}, {}],  # Start l2 with plugin
#         wait_for_announce=True  # Let nodes finish gossip before returning
#     )
#
#     # TODO: need way of setting balances on payment channels
#     completed = []
#     futures = []
#     def trypay(i, sender, receiver):
#         # Small helper initiating a payment and then inserting itself into the
#         # list of completed order in the order of completion.
#         inv = receiver.rpc.invoice(42, "lbl{:}".format(i), "description")['bolt11']
#         sender.rpc.pay(inv)
#         completed.append(i)
#
#     # Queue all payment attempts, and remember the futures.
#     for i in range(10):
#         sender = l1 if i % 2 == 0 else l3
#         receiver = l3 if i % 2 == 0 else l1
#         futures.append(executor.submit(trypay, i, sender, receiver))
#
#     # Now wait for all futures to complete.
#     for f in futures:
#         f.result()
#
#     # Now check that all of them completed, should be FIFO
#     assert(len(completed) == 10)
#     print(completed)
#     #assert(completed == list(range(10)))
