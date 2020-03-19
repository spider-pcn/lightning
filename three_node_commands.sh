#!/bin/sh

#### getting started
# sources the file
source contrib/startup_regtest.sh

# starts bitcoin server plus three nodes
start_ln

# generates new bitcoin address to use as miner for the running instance
bt-cli getnewaddress
# let this output be btaddr

# generate 1000 blocks with the above addr as miner
bt-cli generatetoaddress 1000 <btaddr>

# make sure that you have money on the order of 10k BTC to fund channels
bt-cli getbalance


#### fund the lightning nodes
# generate addresses for wallets for all three lightning nodes so that they can fund channels
l1-cli newaddr # call output's address field l1addr
l2-cli newaddr # call output's address field l2addr
l3-cli newaddr # call output's address field l3addr

# send 1000 BTC to l1, generate blocks to confirm the transaction
bt-cli sendtoaddress <l1addr> 1000
bt-cli generatetoaddress 10 <btaddr> # might need to repear this a few times to see o/p below

# check that l1's wallet has funds
l1-cli listfunds
# should show 10^14 msat in the outputs' field's first amount_msat

# repeat the same process for l2/l3 ensuring they have 10^14 msat also
# don't think changing the number of blocks generated changes its output: you just need to try a few times
bt-cli sendtoaddress <l2addr> 1000
bt-cli generatetoaddress 10 <btaddr> # might need to repear this a few times to see o/p below
l2-cli listfunds
bt-cli sendtoaddress <l3addr> 1000
bt-cli generatetoaddress 10 <btaddr> # might need to repear this a few times to see o/p below
l3-cli listfunds
# as time goes by you need more blocks to confirm - potentially is updating the depth required



#### okay now everyone has money, let's make channels
# Get l2's id and port to open a channel from l1
l2-cli getinfo # call output's id field l2id, port l2port

# establish l1-l2 network connection - all nodes are on same computer, so ip = localhost
l1-cli connect <l2id> localhost <l2port>

# open the channel with 100K sat tokens and a fee rate of 10K sat - throws errors without fee rate
l1-cli fundchannel <l2id> 100000 10000

# generate blocks to confirm the transaction
bt-cli generatetoaddress 100 <btaddr> # might need to repear this a few times to see o/p below

# confirm that channel with peerid = l2id
# has been initialized and its state is "CHANNELD_NORMAL". If it still says,
# "CHANNELD_AWAITING_LOCKIN", you need more blocks to confirm it
l1-cli listfunds 

# l1-cli listchannels also gives you similar output but repeats it twice for bidirectionality
# it is the public view of network topology, so you only get capacity not individual balances

# Repeat the same for l2-l3 channel
l3-cli getinfo  # call output's id field l3id, port l3port
l2-cli connect <l3id> localhost <l3port>
l2-cli fundchannel <l3id> 100000 10000
bt-cli generatetoaddress 100 <btaddr> # might need to repear this a few times to see o/p below

# makes sure l2 has two channels one with l1 and one with l3, both should be "CHANNELD_NORMAL"
# and one extra output for the original btc money stored in the wallet outside channels
# channel with l1 should have 0msat in "our_amount_msat", channel with l3 should have 10^8 msat in
# "our_amount_msat" if you used the above numbers
l2-cli listfunds


### YAY channels set up, let's make payments
# create an invoice at l3 for 1000 msat
l3-cli invoice 1000 test1 firsttest # let output's bolt11 field be l13inv

# decode the payment from l3
l1-cli decodepay <l13inv>

# make a payment from l1 to l3 to this invoice
l1-cli pay <l13inv>
# should say amount_sent to be slightly higher because of fees to l2

# makes sure l2 has two channels one with l1 and one with l3, both should be "CHANNELD_NORMAL"
# and one extra output for the original btc money stored in the wallet outside channels
# channel with l1 should now have 1000msat in "our_amount_msat", channel with l3 should have 
# 10^8 - 1000 msat in "our_amount_msat" if you used the above numbers
# not sure where the extra fees went to though :(
l2-cli listfunds

# if you attempt an l2-l3 payment, as expected no extra money as fees gets sent
l3-cli invoice 1000 test2 secondtest # let output's bolt11 field be l23inv
l2-cli pay <l23inv>
