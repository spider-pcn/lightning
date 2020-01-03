#!/usr/bin/env python3
"""Plugin that holds on to HTLCs for 10 seconds.

Used to test restarts / crashes while HTLCs were accepted, but not yet
settled/forwarded/

"""


from lightning import Plugin, LightningRpc
import json
import os
import tempfile
import time

plugin = Plugin()
MAX_TIME = 10 # minutes
TEMPORARY_CHANNEL_FAILURE = 0x1111


@plugin.hook("htlc_accepted")
def on_htlc_accepted(htlc, onion, plugin, **kwargs):
    plugin.log("htlc_accepted hook called, querying funds")
    entry_time = time.time()

    channel_id = onion["short_channel_id"]
    funds = rpc_interface.listfunds()
    available = sum([int(x["our_amount_msat"]) for x in funds["channels"] \
            if x["short_channel_id"] == channel_id])

    # check if funds are available and wait if not for MAX_TIME
    success = True
    while available < htlc["forward_amt"]:
        time.sleep(100)
        funds = rpc_interface.listfunds()
        available = sum([int(x["our_amount_msat"]) for x in funds["channels"] \
                if x["short_channel_id"] == channel_id])
        
        elapsed_time = time.time() - entry_time
        if elapsed_time.minutes > MAX_TIME:
            success = False
            break

    # forward if successful
    if success:
        return {'result': 'continue'}
    else:
        return {'result': 'fail', 'failure_code': TEMPORARY_CHANNEL_FAILURE}


@plugin.init()
def init(options, configuration, plugin):
    global rpc_interface
    plugin.log("spider_routing.py initializing")
    
    # initialize rpc interface for finding funds available
    basedir = configuration['lightning-dir']
    rpc_filename = configuration['rpc-file']
    path = join(basedir, rpc_filename)
    rpc_interface = LightningRpc(path)
    plugin.log("Funds RPC successfully initialized")


plugin.run()
