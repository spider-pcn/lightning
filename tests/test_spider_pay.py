# A lot of this code has been helped by cdecker

from pyln.testing.fixtures import *
from time import time

plugin_path = {"plugin": os.path.join(os.path.dirname(__file__), "plugins", "spiderpay.py")}

""" make sure regular payment with channel balances goes through
    and increases window
"""
def test_regressive(node_factory, executor):
    l1, l2, l3 = node_factory.line_graph(3, opts=plugin_path, wait_for_announce=True)
    inv = l3.rpc.invoice(42, "lbl{:}".format(int(time())), "description")['bolt11']

    # Let the pay run in the background (on an executor thread) so we don't
    # wait for the pay to succeed before we can check in with the plugin.
    f = executor.submit(l1.rpc.spiderpay, inv)
    
    # Now see that the plugin successfully sends it and updates window
    l1.daemon.wait_for_log(r'amount in flight: 42')
    l1.daemon.wait_for_log(r'attempting to send payment')
    l1.daemon.wait_for_log(r'sendpay_success recorded')
    l1.daemon.wait_for_log(r'adding 0.01 to window')
    l1.daemon.wait_for_log(r'new window is 1000.01')

    # Now retrieve the result from the `pay` task we passed to the executor
    # above. If it failed the exception would get re-raised and fail this
    # test, so just retrieving is enough to check it went through
    f.result()


""" payment with no routes should fail early and not cause side effects
"""
def test_payment_failure(node_factory, executor):
    l1, l2, l3 = node_factory.line_graph(3, opts=plugin_path, wait_for_announce=True)
    inv = l1.rpc.invoice(42, "lbl{:}".format(int(time())), "description")['bolt11']
    f = executor.submit(l3.rpc.spiderpay, inv)
    
    # Now see that the plugin prematurely fails this because there are no routes
    # because insufficient funds below dust in reverse direction
    l3.daemon.wait_for_log(r'no routes')

    # Now retrieve the result from the `pay` task we passed to the executor
    f.result() # TODO: should be fail


""" a payment larger than the minimum window at the start should be queued 
"""
def test_payment_queue(node_factory, executor):
    l1, l2, l3 = node_factory.line_graph(3, opts=plugin_path, wait_for_announce=True)

    # maybe first send successful payment, see increase in window
    # send second payment see decrease
    # third payment > window should be queued

    inv = l3.rpc.invoice(10000000, "lbl{:}".format(int(time())), "description")['bolt11']
    f = executor.submit(l1.rpc.spiderpay, inv)
    l1.daemon.wait_for_log(r'queueing the following payment')


