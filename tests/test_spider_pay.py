# A lot of this code has been helped by cdecker

from pyln.testing.fixtures import *  # noqa: F401,F403
from time import time
import os
import pytest
import logging
import sys

root = logging.getLogger()
root.setLevel(logging.DEBUG)
handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(message)s')
handler.setFormatter(formatter)
root.addHandler(handler)


dirname = os.path.dirname(__file__)
opts = {
    "plugin": os.path.join(dirname, "plugins", "spiderpay.py")
}


def test_regressive(node_factory, executor):
    """make sure regular payment with channel balances goes through and increases
    window

    """
    l1, l2, l3 = node_factory.line_graph(3, opts=opts, wait_for_announce=True)
    inv = l3.rpc.invoice(42, "lbl", "description")['bolt11']

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


def test_payment_failure(node_factory, executor):
    """ payment with no routes should fail early and not cause side effects.
    """
    l1, l2, l3 = node_factory.line_graph(3, opts=opts, wait_for_announce=True)
    inv = l1.rpc.invoice(42, "lbl", "description")['bolt11']
    f = executor.submit(l3.rpc.spiderpay, inv)

    # Now see that the plugin prematurely fails this because there are no
    # routes because insufficient funds below dust in reverse direction
    l3.daemon.wait_for_log(r'no routes')

    # Now retrieve the result from the `pay` task we passed to the executor
    # f.result() # TODO: should be fail
    with pytest.raises():
        f.result()


def test_payment_queue(node_factory, executor):
    """a payment larger than the minimum window at the start should be queued

    """
    l1, l2, l3 = node_factory.line_graph(3, opts=opts, wait_for_announce=True)

    # maybe first send successful payment, see increase in window
    # send second payment see decrease
    # third payment > window should be queued

    inv = l3.rpc.invoice(10000000, "lbl", "description")['bolt11']
    f = executor.submit(l1.rpc.spiderpay, inv)
    l1.daemon.wait_for_log(r'queueing the following payment')
    f.result()


def test_multiple_routes(node_factory, executor):
    """ set up a test involving multiple routes between sender/destination

    SETUP: A basic circular setup to set up two paths

    l1---l2
    |    |
    l4---l3

    """
    l1, l2, l3, l4 = node_factory.line_graph(4, opts=opts,
                                             wait_for_announce=True)
    l4.rpc.connect(l1.info['id'], 'localhost', l1.port)

    # attempt an l1 - l3 payment
    inv1 = l3.rpc.invoice(40, "lbl1", "description")['bolt11']
    f1 = executor.submit(l1.rpc.spiderpay, inv1)

    # should collect two paths
    l1.daemon.wait_for_log(r'found 2 routes to destination')
    l1.daemon.wait_for_log(r'amount in flight: 40 on route 0')

    # attempt a second payment - TODO: make sure it goes on the second path
    inv2 = l3.rpc.invoice(80, "lbl2", "description")['bolt11']
    f2 = executor.submit(l1.rpc.spiderpay, inv2)
    l1.daemon.wait_for_log(r'amount in flight: 80 on route 1')

    # attempt a third payment that should get queued
    # TODO: set up windows correctly
    inv3 = l3.rpc.invoice(120, "lbl3", "description")['bolt11']
    f3 = executor.submit(l1.rpc.spiderpay, inv3)
    l1.daemon.wait_for_log(r'queueing the following payment')

    # once the windows get increased after the first two payments succeed
    # this payment should go through
    f1.result()
    f2.result()
    f3.result()

    # check for correct window increases TODO
