def try_payment_on_path(plugin, best_route_index, amount, destination, payment_hash):
    #by here best_route is the route to send the unit on
    #update the value in the route info
    plugin.routes_in_use[destination][best_route_index][2] += amount

    #now we actually pay the one unit on that route
    try:
        plugin.rpc.sendpay(route, payment_hash)
        plugin.rpc.waitsendpay(payment_hash, retry_for + start_ts - int(time.time()))
        handle_result(plugin, False, best_route_index, destination)
        return success_msg

    except RpcError as e:
        plugin.log("RpcError: " + str(e))
        handle_result(plugin, True, best_route_index, destination)
        return failure_msg

def send_more_transactions(plugin, destination, route_index):
    route = plugin.routes_in_use[destination][route_index]
    slack = route[1] - route[2]
    while len(plugin.queue[destination]) > 0:
        oldest_payment = plugin.queue[destination][0]
        amount = None #TODO
        payment_hash = None #TODO
        if amount <= slack:
            try_payment_on_path(plugin, route_index, amount, destination, payment_hash)
        else:
            break

def handle_result(plugin, marked, route_index, destination, amount):
    #Need something to update weights
    #when we get a response as marked or not marked'
    route = routes_in_use[destination][route_index]
    route[2] -= amount
    if marked:
        route[1] = max(route[2] - plugin.beta, 1)
    else:
        summation = 0
        for routes in plugin.routes_in_use[destination]:
            summation += routes[1]
        route[1] += (plugin.alpha/summation)
        slack = route_info[1] - route_info[2]
        send_more_transactions(plugin, destination, route_index)



@plugin.method('spider_pay')
def spider_pay(plugin, invoice):
    #TODO
    destination = None
    amount = None
    payment_hash = invoice['payment_hash']

    if destination not in plugin.routes_in_use:
        plugin.routes_in_use[destination] = []
        #routes = find all possible edge disjoint widest paths (maximum minimum weight on a path) (choose 4)
        excludes = []
        window = 1

        for i in range(4):
            r = plugin.rpc.getroute(destination, plugin.payment_size, riskfactor=1,
                                    cltv=9, exclude=excludes)
            excludes += r['route']
            plugin.routes_in_use[destination].append((r['route'], window, 0))

    if plugin.routes_in_use[destination] == []:
        return failure_msg

    best_route = None
    #slack is how much we can send minus how much we are sending
    best_route_slack = None
    best_route_index = None
    #find biggest gap between window and what is being sent
    for i, route_info in enumerate(random_shuffle(plugin.routes_in_use[destination])):
        slack = route_info[1] - route_info[2]
        if slack >= amount:
            best_route_slack = slack
            best_route = route_info[0]
            best_route_index = i
            break

    if best_route is None:
        if destination in plugin.queue:
            plugin.queue[destination].append(invoice)
        else:
            plugin.queue[destination] = dequeue()
            plugin.queue.append(invoice)
        return

    return try_payment_on_path(plugin, best_route_index, amount, destination, payment_hash)

@plugin.init()
def init(options, configuration, plugin):
    plugin.log("spider_pay.py initializing")

    #maps destination to list of paths each of which has some score
    #window corresponding to its max capacity and how much we use it
    plugin.routes_in_use = {}
    #turn this into a dictionary of dequeues
    plugin.queue = {}
    plugin.beta = 1
    plugin.alpha = 10

    # Now start the background thread that'll trickle the HTLCs
    # through. daemon=True makes sure that we don't wait for the thread to
    # exit when shutting down.
    thread = Thread(target=clear_pending, args=(plugin,), daemon=True)
    thread.start()

plugin.run()
