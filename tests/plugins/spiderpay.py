#!/usr/bin/env python3
"""Plugin that queues transactions

Used to test restarts / crashes while HTLCs were accepted, but not yet
settled/forwarded/

"""


from lightning import Plugin, RpcError
from threading import Thread
from collections import deque
import random
import time

plugin = Plugin()

THRESHOLD = 100000 #Msats below which we exclude channels from re-consideration
MAX_PATHS = 4
MIN_WINDOW = 1000

""" update inflight and attempt transaction on specified route 
"""
def try_payment_on_path(plugin, best_route_index, amount, destination, payment_hash):
    route_info = plugin.routes_in_use[destination][best_route_index]
    route_info["amount_inflight"] += amount
    plugin.log("amount in flight: {} on route {}".format(route_info["amount_inflight"], best_route_index))

    plugin.payment_hash_to_route[payment_hash] = best_route_index
    print("attempting to send payment", payment_hash,
               "on route ", route_info["route"])
    plugin.rpc.sendpay(route_info["route"], payment_hash)


""" send more transactions to this destination on this route 
    because some window just became available 
"""
def send_more_transactions(plugin, destination, route_index):
    route_info = plugin.routes_in_use[destination][route_index]
    slack = route_info["window"] - route_info["amount_inflight"]
    while len(plugin.queue.get(destination, [])) > 0:
        oldest_payment = plugin.queue[destination][0]
        amount = oldest_payment['amount_msat']
        payment_hash = oldest_payment['payment_hash']
        if amount <= slack:
            try_payment_on_path(plugin, route_index, amount,
                                destination, payment_hash)
        else:
            break


""" when a payment successfully completes on any route,
    update amount inflight, increase window and send 
    more payments on that route 
"""
@plugin.subscribe("sendpay_success")
def handle_sendpay_success(plugin, sendpay_success, **kwargs):
    plugin.log("sendpay_success recorded, id: {},\
                payment_hash: {}".format(sendpay_success['id'],
                sendpay_success['payment_hash'])
            )
    
    # update inflight
    destination = sendpay_success['destination']
    amount = sendpay_success['msatoshi']
    payment_hash = sendpay_success['payment_hash']
    route_index = plugin.payment_hash_to_route[payment_hash]
    route_info = plugin.routes_in_use[destination][route_index]
    route_info["amount_inflight"] -= amount
    plugin.log("amount in flight: {}".format(route_info["amount_inflight"]))

    # update window
    summation = 0
    for routes in plugin.routes_in_use[destination]:
        summation += routes["window"]
    plugin.log("adding {} to window on route {}".format(plugin.alpha/summation, route_info["route"]))
    route_info["window"] += (plugin.alpha/summation)
    plugin.log("new window is {}".format(route_info["window"]))
    del plugin.payment_hash_to_route[payment_hash]

    send_more_transactions(plugin, destination, route_index)


""" when a payment fails on any route,
    update amount inflight and decrease window
"""
@plugin.subscribe("sendpay_failure")
def handle_sendpay_failure(plugin, sendpay_failure, **kwargs):
    plugin.log("sendpay_failure recorded, id: {},\
     payment_hash: {}".format(sendpay_failure['data']['id'],
               sendpay_failure['data']['payment_hash']))

    # update inflight
    destination = sendpay_failure['data']['destination']
    amount = sendpay_failure['data']['msatoshi']
    payment_hash = sendpay_failure['data']['payment_hash']
    route_index = plugin.payment_hash_to_route[payment_hash]
    route_info = plugin.routes_in_use[destination][route_index]
    route_info["amount_inflight"] -= amount
    plugin.log("amount in flight: {}".format(route_info["amount_inflight"]))

    # update window
    plugin.log("removing {} from window on route {}".format(plugin.beta, route_info["route"]))
    route_info["window"] = max(route_info["amount_inflight"] - plugin.beta, MIN_WINDOW)
    plugin.log("new window is {}".format(route_info["window"]))
    del plugin.payment_hash_to_route[payment_hash]


""" main plugin method responsible for initiating payments
    by finding routes to the destination and picking the 
    destination with most available window 
"""
@plugin.async_method('spiderpay')
def spiderpay(plugin, invoice):
    decoded = plugin.rpc.decodepay(invoice)
    destination = decoded['payee']
    amount = decoded['msatoshi']
    payment_hash = decoded['payment_hash']
    plugin.log("starting to call spiderpay for invoice {} to destination {},\
               for amount {} with payment hash {}".format(invoice, destination,
                                                          amount, payment_hash))

    # find a set of routes to the destination
    if destination not in plugin.routes_in_use:
        plugin.routes_in_use[destination] = []
        excludes = []
        print("about to find routes")

        for i in range(MAX_PATHS):
            try:
                r = plugin.rpc.getroute(destination, amount,
                                    riskfactor=1, cltv=9, exclude=excludes)
                print("found the #", i, " route: ", r)
                route_info = {"route": r['route'],
                              "window": MIN_WINDOW,
                              "amount_inflight": 0
                }
                plugin.routes_in_use[destination].append(route_info)

                for c in r['route']:
                    if 'channel' in c and c['msatoshi'] < THRESHOLD:
                        excludes.append(c['channel'] + '/' + str(c['direction']))
                    print ("excludes: ", excludes)
            
            except RpcError as e:
                plugin.log("RPC Error " + str(e))
                break

    if plugin.routes_in_use[destination] == []:
        plugin.log("no routes to destination:{} for invoice {}".format(destination, invoice))
        # should fail TODO
        return

    plugin.log("found {} routes to destination {} ".format(len(plugin.routes_in_use[destination]), 
        destination))

    # queue up if there's already payments to this destination that are queued
    if destination in plugin.queue and len(plugin.queue[destination]) > 0:
        plugin.queue[destination].append(invoice)
        return

    # find the route with the biggest gap between window and what is being sent
    best_route = None
    best_route_slack = None
    best_route_index = None
    random.shuffle(plugin.routes_in_use[destination])
    for i, route_info in enumerate(plugin.routes_in_use[destination]):
        slack = route_info["window"] - route_info["amount_inflight"]
        if slack >= amount:
            best_route_slack = slack
            best_route = route_info["route"]
            best_route_index = i
            break

    if best_route is None:
        if destination in plugin.queue:
            plugin.queue[destination].append(invoice)
        else:
            plugin.queue[destination] = deque()
            plugin.log("queueing the following payment: {}".format(payment_hash))
            plugin.queue[destination].append(invoice)
        return

    try_payment_on_path(plugin, best_route_index, amount,
                               destination, payment_hash)


@plugin.method('spider-inspect')
def inspect(plugin):
    return {
        "queue": plugin.queue,
        "routes_in_use": plugin.routes_in_use,
    }


""" initializes the plugin """
@plugin.init()
def init(options, configuration, plugin):
    plugin.log("spiderpay.py initializing")

    # maps destination to list of paths each of which has some score
    # window corresponding to its max capacity and how much we use it
    plugin.routes_in_use = {}
    
    # per-destination queue at the sender of payments to be sent
    plugin.queue = {}

    # maps the payments already sent out to the route index that they were sent on
    plugin.payment_hash_to_route = {}

    # window decrease and increase factors
    plugin.beta = 1
    plugin.alpha = 10

plugin.run()
