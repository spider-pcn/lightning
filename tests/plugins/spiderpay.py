#!/usr/bin/env python3
"""Plugin that queues transactions

Used to test restarts / crashes while HTLCs were accepted, but not yet
settled/forwarded/

"""


from lightning import Plugin
from threading import Thread
from collections import deque
import time

plugin = Plugin()

THRESHOLD = 1000 #Msats below which we exclude channels from re-consideration
def try_payment_on_path(plugin, best_route_index, amount, destination, payment_hash):
    #by here best_route is the route to send the unit on
    #update the value in the route info
    route_info = plugin.routes_in_use[destination][best_route_index]
    route_info["amount_inflight"] += amount

    #now we actually pay the one unit on that route
    plugin.payment_hash_to_route[payment_hash] = best_route_index
    plugin.log("attempting to send payment", payment_hash,
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

    #update window
    summation = 0
    for routes in plugin.routes_in_use[destination]:
        summation += routes["window"]
    plugin.log("adding ", plugin.alpha/summation, " to window on route ",
               route_info["route"])
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

    #update window
    plugin.log("removing ", plugin.beta, " from window on route ", route_info["route"])
    route_info["window"] = max(route_info["amount_inflight"] - plugin.beta, 1)
    del plugin.payment_hash_to_route[payment_hash]


@plugin.async_method('spiderpay')
def spiderpay(plugin, invoice):
    plugin.log("starting to call spiderpay")
    plugin.log("invoice: ", invoice)
    decoded = plugin.rpc.decodepay(invoice)
    plugin.log("type of result: ", type(decoded))
    plugin.log("decodepay of invoice: ", decoded)
    destination = decoded['payee']
    plugin.log("destination: ", destination)
    amount = invoice['amount_msat']
    plugin.log("amount: ", amount)
    payment_hash = invoice['payment_hash']
    plugin.log("payment_hash: ", payment_hash)

    if destination not in plugin.routes_in_use:
        plugin.routes_in_use[destination] = []
        #routes = find all possible edge disjoint widest paths
        #(maximum minimum weight on a path) (choose 4)
        excludes = []
        window = 1000000
        plugin.log("about to find routes")

        for i in range(4):
            r = plugin.rpc.getroute(destination, plugin.payment_size,
                                    riskfactor=1, cltv=9, exclude=excludes)
            plugin.log("found a route: ", i)
            route_info = {"route": r,
                          "window": window,
                          "amount_inflight": 0
            }
            plugin.routes_in_use[destination].append(route_info)

            for c in route:
                if c['channel']['amount_msat'] < THRESHOLD:
                    if 'short_channel_id' in c['channel']:
	                       excludes.append(c['channel']['short_channel_id'])

    if plugin.routes_in_use[destination] == []:
        plugin.log("no routes to destination: ", destination)
        return failure_msg

    best_route = None
    #slack is how much we can send minus how much we are sending
    best_route_slack = None
    best_route_index = None
    #find biggest gap between window and what is being sent
    for i, route_info in enumerate(random_shuffle(plugin.routes_in_use[destination])):
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
            plugin.queue[destination] = dequeue()
            plugin.log("queueing the following payment: ", invoice["payment_hash"])
            plugin.queue.append(invoice)
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

    # Now start the background thread that'll trickle the HTLCs
    # through. daemon=True makes sure that we don't wait for the thread to
    # exit when shutting down.
    # TODO: do we need a cleanup thread?
    #thread = Thread(target=clear_pending, args=(plugin,), daemon=True)
    #thread.start()

plugin.run()
