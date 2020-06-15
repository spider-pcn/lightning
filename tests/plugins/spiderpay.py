#!/usr/bin/env python3
"""Plugin that queues transactions

Used to test restarts / crashes while HTLCs were accepted, but not yet
settled/forwarded/

"""


from lightning import Plugin
from threading import Thread
from collections import deque
import random
import time

plugin = Plugin()

THRESHOLD = 100000 #Msats below which we exclude channels from re-consideration
MAX_PATHS = 1
MIN_WINDOW = 1000

def try_payment_on_path(plugin, best_route_index, amount, destination, payment_hash):
    #by here best_route is the route to send the unit on
    #update the value in the route info
    route_info = plugin.routes_in_use[destination][best_route_index]
    route_info["amount_inflight"] += amount
    plugin.log("amount in flight: {}".format(route_info["amount_inflight"]))

    #now we actually pay the one unit on that route
    plugin.payment_hash_to_route[payment_hash] = best_route_index
    print("attempting to send payment", payment_hash,
               "on route ", route_info["route"])
    plugin.rpc.sendpay(route_info["route"], payment_hash)

def send_more_transactions(plugin, destination, route_index):
    route_info = plugin.routes_in_use[destination][route_index]
    slack = route_info["window"] - route_info["amount_inflight"]
    while len(plugin.queue[destination]) > 0:
        oldest_payment = plugin.queue[destination][0]
        amount = oldest_payment['amount_msat']
        payment_hash = oldest_payment['payment_hash']
        if amount <= slack:
            try_payment_on_path(plugin, route_index, amount,
                                destination, payment_hash)
        else:
            break

@plugin.subscribe("sendpay_success")
def handle_sendpay_success(plugin, sendpay_success):
    plugin.log("receive a sendpay_success recorded, id: {},\
                payment_hash: {}".format(sendpay_success['id'],
                sendpay_success['payment_hash'])
            )
    destination = sendpay_success['destination']
    amount = sendpay_success['msatoshi']
    payment_hash = sendpay_success['payment_hash']
    route_index = plugin.payment_hash_to_route[payment_hash]
    route_info = plugin.routes_in_use[destination][route_index]
    route_info["amount_inflight"] -= amount
    plugin.log("amount in flight: {}".format(route_info["amount_inflight"]))

    #update window
    summation = 0
    for routes in plugin.routes_in_use[destination]:
        summation += routes["window"]
    plugin.log("adding {} to window on route {}".format(plugin.alpha/summation, route_info["route"]))
    route_info["window"] += (plugin.alpha/summation)
    slack = route_info["window"] - route_info["amount_inflight"]
    del plugin.payment_hash_to_route[payment_hash]

    send_more_transactions(plugin, destination, route_index)


@plugin.subscribe("sendpay_failure")
def handle_sendpay_failure(plugin, sendpay_failure):
    plugin.log("receive a sendpay_failure recorded, id: {},\
     payment_hash: {}".format(sendpay_failure['data']['id'],
               sendpay_failure['data']['payment_hash']))

    destination = sendpay_failure['data']['destination']
    amount = sendpay_failure['data']['msatoshi']
    payment_hash = sendpay_failure['data']['payment_hash']
    route_index = plugin.payment_hash_to_route[payment_hash]
    route_info = plugin.routes_in_use[destination][route_index]
    route_info["amount_inflight"] -= amount
    plugin.log("amount in flight: {}".format(route_info["amount_inflight"]))

    #update window
    plugin.log("removing {} from window on route {}".format(plugin.beta, route_info["route"]))
    route_info["window"] = max(route_info["amount_inflight"] - plugin.beta, MIN_WINDOW)
    plugin.log("new window is {}".format(route_info["window"]))
    del plugin.payment_hash_to_route[payment_hash]


@plugin.async_method('spiderpay')
def spiderpay(plugin, invoice):
    decoded = plugin.rpc.decodepay(invoice)
    destination = decoded['payee']
    amount = decoded['msatoshi']
    payment_hash = decoded['payment_hash']
    plugin.log("starting to call spiderpay for invoice {} to destination {},\
               for amount {} with payment hash {}".format(invoice, destination,
                                                          amount, payment_hash))

    if destination not in plugin.routes_in_use:
        plugin.routes_in_use[destination] = []
        #routes = find all possible edge disjoint widest paths
        #(maximum minimum weight on a path) (choose 4)
        excludes = []
        print("about to find routes")

        for i in range(MAX_PATHS):
            r = plugin.rpc.getroute(destination, amount,
                                    riskfactor=1, cltv=9, exclude=excludes)
            print("found the #", i, " route: ", r)
            route_info = {"route": r['route'],
                          "window": MIN_WINDOW,
                          "amount_inflight": 0
            }
            plugin.routes_in_use[destination].append(route_info)

            for c in r['route']:
                if 'short_channel_id' in c and c['msatoshi'] < THRESHOLD:
                    excludes.append(c['short_channel_id'] + '/' + str(c['direction']))
                print ("excludes: ", excludes)

    if plugin.routes_in_use[destination] == []:
        print("no routes to destination: ", destination)
        return failure_msg

    print ("routes found: ", plugin.routes_in_use[destination])

    best_route = None
    #slack is how much we can send minus how much we are sending
    best_route_slack = None
    best_route_index = None
    #find biggest gap between window and what is being sent
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

    return try_payment_on_path(plugin, best_route_index, amount,
                               destination, payment_hash)

@plugin.init()
def init(options, configuration, plugin):
    plugin.log("spiderpay.py initializing")

    #maps destination to list of paths each of which has some score
    #window corresponding to its max capacity and how much we use it
    plugin.routes_in_use = {}
    #turn this into a dictionary of dequeues
    plugin.queue = {}
    plugin.payment_hash_to_route = {}
    plugin.beta = 1
    plugin.alpha = 10

plugin.run()
