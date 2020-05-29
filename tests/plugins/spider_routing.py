#!/usr/bin/env python3
"""Plugin that holds on to HTLCs for 10 seconds.

Used to test restarts / crashes while HTLCs were accepted, but not yet
settled/forwarded/

"""


from lightning import Plugin, LightningRpc
from threading import Thread
from collections import deque
import json
import os
import tempfile
import time

plugin = Plugin()
MAX_TIME = 10 # minutes
TEMPORARY_CHANNEL_FAILURE = 0x1111


# queue the payment if there is already a queue or if there 
# aren't enough funds
@plugin.async_hook("htlc_accepted")
def on_htlc_accepted(onion, htlc, request, plugin, **kwargs):
    entry_time = time.time()
    channel_id = onion["short_channel_id"]
    if channel_id not in plugin.pending:
        plugin.pending[channel_id] = deque()
    
    plugin.log("Stashing HTLC {}, next short_channel_id {} now have {} pending HTLCs".format(
        channel_id, 
        htlc['payment_hash'],
        len(plugin.pending[channel_id]),
    ))

    if len(plugin.pending[channel_id]) > 0:
        plugin.pending[channel_id].append((request, entry_time))
    else:
        funds = rpc_interface.listfunds()
        available = sum([int(x["our_amount_msat"]) for x in funds["channels"] \
                if x["short_channel_id"] == channel_id])
        if available < htlc["forward_amt"]:
            plugin.pending[channel_id].append((request, entry_time))
        else:
            return {'result': 'continue'}

# background thread that goes through the queues and tries the next payment in LIFO order 
# and also garbage collects old transactions
def clear_pending(plugin):
    while True:
        time.sleep(5)

        for channel_id, request_queue in plugin.pending.items():
            # first clear old/timed out requests
            while len(request_queue) > 0:
                oldest_entry_time = request_queue[-1][1]
                elapsed_time = time.time() - oldest_entry_time 

                if elapsed_time.minutes > MAX_TIME:
                    oldest_request = request_queue.popleft()[0]
                    plugin.log("Failing HTLC {} through (request {})".format(
                        oldest_request.params['htlc']['payment_hash'],
                        oldest_request.id
                    ))
                    oldest_request.set_result({
                        'result': 'fail', 
                        'failure_code': TEMPORARY_CHANNEL_FAILURE
                        })
                else:
                    break

            # If we don't have any pending requests go back to sleep
            if len(request_queue) == 0:
                continue

            # Pick the most recent payment (LIFO works best for deadlines)
            # TODO: but not sure if we have deadlines here
            # (remove payment so that you don't respond again to same request 
            # in future and get killed by c-lightning because of that).
            request = request_queue.pop()[0]
            
            # log info including from the original request
            print(len(request_queue))        
            plugin.log("Allowing HTLC {} through (request {})".format(
                request.params['htlc']['payment_hash'],
                request.id
            ))

            # We mostly want to let c-lightning handle forwarding, so tell it to
            # continue whatever it'd do next anyway. 
            request.set_result({'result': 'continue'})


@plugin.init()
def init(options, configuration, plugin):
    global rpc_interface
    plugin.log("spider_routing.py initializing")
    
    # dictionary of list of pending requests indexed by next channel
    # or short channel id.
    plugin.pending = {}

    # initialize rpc interface for finding funds available
    basedir = configuration['lightning-dir']
    rpc_filename = configuration['rpc-file']
    path = join(basedir, rpc_filename)
    rpc_interface = LightningRpc(path)
    plugin.log("Funds RPC successfully initialized")

    # Now start the background thread that'll trickle the HTLCs
    # through. daemon=True makes sure that we don't wait for the thread to
    # exit when shutting down.
    thread = Thread(target=clear_pending, args=(plugin,), daemon=True)
    thread.start()

plugin.run()
