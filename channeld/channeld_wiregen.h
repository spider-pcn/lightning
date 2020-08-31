/* This file was generated by generate-wire.py */
/* Do not modify this file! Modify the _csv file it was generated from. */
/* Original template can be found at tools/gen/header_template */

#ifndef LIGHTNING_CHANNELD_CHANNELD_WIREGEN_H
#define LIGHTNING_CHANNELD_CHANNELD_WIREGEN_H
#include <ccan/tal/tal.h>
#include <wire/tlvstream.h>
#include <wire/wire.h>
#include <common/cryptomsg.h>
#include <common/channel_config.h>
#include <common/derive_basepoints.h>
#include <common/features.h>
#include <common/fee_states.h>
#include <common/per_peer_state.h>
#include <bitcoin/preimage.h>
#include <common/penalty_base.h>
#include <common/htlc_wire.h>

enum channeld_wire {
        /*  Begin!  (passes gossipd-client fd) */
        WIRE_CHANNELD_INIT = 1000,
        /*  master->channeld funding hit new depth(funding locked if >= lock depth) */
        WIRE_CHANNELD_FUNDING_DEPTH = 1002,
        /*  Tell channel to offer this htlc */
        WIRE_CHANNELD_OFFER_HTLC = 1004,
        /*  Reply; synchronous since IDs have to increment. */
        WIRE_CHANNELD_OFFER_HTLC_REPLY = 1104,
        /*  Main daemon found out the preimage for an HTLC */
        WIRE_CHANNELD_FULFILL_HTLC = 1005,
        /*  Main daemon says HTLC failed */
        WIRE_CHANNELD_FAIL_HTLC = 1006,
        /*  When we receive funding_locked. */
        WIRE_CHANNELD_GOT_FUNDING_LOCKED = 1019,
        /*  When we send a commitment_signed message */
        WIRE_CHANNELD_SENDING_COMMITSIG = 1020,
        /*  Wait for reply */
        WIRE_CHANNELD_SENDING_COMMITSIG_REPLY = 1120,
        /*  When we have a commitment_signed message */
        WIRE_CHANNELD_GOT_COMMITSIG = 1021,
        /*  Wait for reply */
        WIRE_CHANNELD_GOT_COMMITSIG_REPLY = 1121,
        WIRE_CHANNELD_GOT_REVOKE = 1022,
        /*  Wait for reply */
        /*  (eg. if we sent another commitment_signed */
        WIRE_CHANNELD_GOT_REVOKE_REPLY = 1122,
        /*  Tell peer to shut down channel. */
        WIRE_CHANNELD_SEND_SHUTDOWN = 1023,
        /*  Peer told us that channel is shutting down */
        WIRE_CHANNELD_GOT_SHUTDOWN = 1024,
        /*  Shutdown is complete */
        WIRE_CHANNELD_SHUTDOWN_COMPLETE = 1025,
        /*  Re-enable commit timer. */
        WIRE_CHANNELD_DEV_REENABLE_COMMIT = 1026,
        WIRE_CHANNELD_DEV_REENABLE_COMMIT_REPLY = 1126,
        WIRE_CHANNELD_FEERATES = 1027,
        /*  master -> channeld: do you have a memleak? */
        WIRE_CHANNELD_DEV_MEMLEAK = 1033,
        WIRE_CHANNELD_DEV_MEMLEAK_REPLY = 1133,
        /*  Peer presented proof it was from the future. */
        WIRE_CHANNELD_FAIL_FALLEN_BEHIND = 1028,
        /*  Handle a channel specific feerate base ppm configuration */
        WIRE_CHANNELD_SPECIFIC_FEERATES = 1029,
        /*  When we receive announcement_signatures for channel announce */
        WIRE_CHANNELD_GOT_ANNOUNCEMENT = 1017,
        /*  Ask channeld to send a error message. Used in forgetting channel case. */
        WIRE_CHANNELD_SEND_ERROR = 1008,
        /*  Tell master channeld has sent the error message. */
        WIRE_CHANNELD_SEND_ERROR_REPLY = 1108,
        /*  Tell lightningd we got a onion message (for us */
        WIRE_GOT_ONIONMSG_TO_US = 1142,
        WIRE_GOT_ONIONMSG_FORWARD = 1143,
        /*  Lightningd tells us to send a onion message. */
        WIRE_SEND_ONIONMSG = 1040,
};

const char *channeld_wire_name(int e);

/**
 * Determine whether a given message type is defined as a message.
 *
 * Returns true if the message type is part of the message definitions we have
 * generated parsers for, false if it is a custom message that cannot be
 * handled internally.
 */
bool channeld_wire_is_defined(u16 type);


/* WIRE: CHANNELD_INIT */
/*  Begin!  (passes gossipd-client fd) */
u8 *towire_channeld_init(const tal_t *ctx, const struct chainparams *chainparams, const struct feature_set *our_features, const struct bitcoin_txid *funding_txid, u16 funding_txout, struct amount_sat funding_satoshi, u32 minimum_depth, const struct channel_config *our_config, const struct channel_config *their_config, const struct fee_states *fee_states, u32 feerate_min, u32 feerate_max, u32 feerate_penalty, const struct bitcoin_signature *first_commit_sig, const struct per_peer_state *per_peer_state, const struct pubkey *remote_fundingkey, const struct basepoints *remote_basepoints, const struct pubkey *remote_per_commit, const struct pubkey *old_remote_per_commit, enum side opener, u32 fee_base, u32 fee_proportional, struct amount_msat local_msatoshi, const struct basepoints *our_basepoints, const struct pubkey *our_funding_pubkey, const struct node_id *local_node_id, const struct node_id *remote_node_id, u32 commit_msec, u16 cltv_delta, bool last_was_revoke, const struct changed_htlc *last_sent_commit, u64 next_index_local, u64 next_index_remote, u64 revocations_received, u64 next_htlc_id, const struct existing_htlc **htlcs, bool local_funding_locked, bool remote_funding_locked, const struct short_channel_id *funding_short_id, bool reestablish, bool send_shutdown, bool remote_shutdown_received, const u8 *final_scriptpubkey, u8 flags, const u8 *init_peer_pkt, bool reached_announce_depth, const struct secret *last_remote_secret, const u8 *their_features, const u8 *upfront_shutdown_script, const secp256k1_ecdsa_signature *remote_ann_node_sig, const secp256k1_ecdsa_signature *remote_ann_bitcoin_sig, bool option_static_remotekey, bool option_anchor_outputs, bool dev_fast_gossip, bool dev_fail_process_onionpacket, const struct penalty_base *pbases);
bool fromwire_channeld_init(const tal_t *ctx, const void *p, const struct chainparams **chainparams, struct feature_set **our_features, struct bitcoin_txid *funding_txid, u16 *funding_txout, struct amount_sat *funding_satoshi, u32 *minimum_depth, struct channel_config *our_config, struct channel_config *their_config, struct fee_states **fee_states, u32 *feerate_min, u32 *feerate_max, u32 *feerate_penalty, struct bitcoin_signature *first_commit_sig, struct per_peer_state **per_peer_state, struct pubkey *remote_fundingkey, struct basepoints *remote_basepoints, struct pubkey *remote_per_commit, struct pubkey *old_remote_per_commit, enum side *opener, u32 *fee_base, u32 *fee_proportional, struct amount_msat *local_msatoshi, struct basepoints *our_basepoints, struct pubkey *our_funding_pubkey, struct node_id *local_node_id, struct node_id *remote_node_id, u32 *commit_msec, u16 *cltv_delta, bool *last_was_revoke, struct changed_htlc **last_sent_commit, u64 *next_index_local, u64 *next_index_remote, u64 *revocations_received, u64 *next_htlc_id, struct existing_htlc ***htlcs, bool *local_funding_locked, bool *remote_funding_locked, struct short_channel_id *funding_short_id, bool *reestablish, bool *send_shutdown, bool *remote_shutdown_received, u8 **final_scriptpubkey, u8 *flags, u8 **init_peer_pkt, bool *reached_announce_depth, struct secret *last_remote_secret, u8 **their_features, u8 **upfront_shutdown_script, secp256k1_ecdsa_signature **remote_ann_node_sig, secp256k1_ecdsa_signature **remote_ann_bitcoin_sig, bool *option_static_remotekey, bool *option_anchor_outputs, bool *dev_fast_gossip, bool *dev_fail_process_onionpacket, struct penalty_base **pbases);

/* WIRE: CHANNELD_FUNDING_DEPTH */
/*  master->channeld funding hit new depth(funding locked if >= lock depth) */
u8 *towire_channeld_funding_depth(const tal_t *ctx, const struct short_channel_id *short_channel_id, u32 depth);
bool fromwire_channeld_funding_depth(const tal_t *ctx, const void *p, struct short_channel_id **short_channel_id, u32 *depth);

/* WIRE: CHANNELD_OFFER_HTLC */
/*  Tell channel to offer this htlc */
u8 *towire_channeld_offer_htlc(const tal_t *ctx, struct amount_msat amount_msat, u32 cltv_expiry, const struct sha256 *payment_hash, const u8 onion_routing_packet[1366], const struct pubkey *blinding);
bool fromwire_channeld_offer_htlc(const tal_t *ctx, const void *p, struct amount_msat *amount_msat, u32 *cltv_expiry, struct sha256 *payment_hash, u8 onion_routing_packet[1366], struct pubkey **blinding);

/* WIRE: CHANNELD_OFFER_HTLC_REPLY */
/*  Reply; synchronous since IDs have to increment. */
u8 *towire_channeld_offer_htlc_reply(const tal_t *ctx, u64 id, const u8 *failuremsg, const wirestring *failurestr);
bool fromwire_channeld_offer_htlc_reply(const tal_t *ctx, const void *p, u64 *id, u8 **failuremsg, wirestring **failurestr);

/* WIRE: CHANNELD_FULFILL_HTLC */
/*  Main daemon found out the preimage for an HTLC */
u8 *towire_channeld_fulfill_htlc(const tal_t *ctx, const struct fulfilled_htlc *fulfilled_htlc);
bool fromwire_channeld_fulfill_htlc(const void *p, struct fulfilled_htlc *fulfilled_htlc);

/* WIRE: CHANNELD_FAIL_HTLC */
/*  Main daemon says HTLC failed */
u8 *towire_channeld_fail_htlc(const tal_t *ctx, const struct failed_htlc *failed_htlc);
bool fromwire_channeld_fail_htlc(const tal_t *ctx, const void *p, struct failed_htlc **failed_htlc);

/* WIRE: CHANNELD_GOT_FUNDING_LOCKED */
/*  When we receive funding_locked. */
u8 *towire_channeld_got_funding_locked(const tal_t *ctx, const struct pubkey *next_per_commit_point);
bool fromwire_channeld_got_funding_locked(const void *p, struct pubkey *next_per_commit_point);

/* WIRE: CHANNELD_SENDING_COMMITSIG */
/*  When we send a commitment_signed message */
u8 *towire_channeld_sending_commitsig(const tal_t *ctx, u64 commitnum, const struct penalty_base *pbase, const struct fee_states *fee_states, const struct changed_htlc *changed, const struct bitcoin_signature *commit_sig, const struct bitcoin_signature *htlc_sigs);
bool fromwire_channeld_sending_commitsig(const tal_t *ctx, const void *p, u64 *commitnum, struct penalty_base **pbase, struct fee_states **fee_states, struct changed_htlc **changed, struct bitcoin_signature *commit_sig, struct bitcoin_signature **htlc_sigs);

/* WIRE: CHANNELD_SENDING_COMMITSIG_REPLY */
/*  Wait for reply */
u8 *towire_channeld_sending_commitsig_reply(const tal_t *ctx);
bool fromwire_channeld_sending_commitsig_reply(const void *p);

/* WIRE: CHANNELD_GOT_COMMITSIG */
/*  When we have a commitment_signed message */
u8 *towire_channeld_got_commitsig(const tal_t *ctx, u64 commitnum, const struct fee_states *fee_states, const struct bitcoin_signature *signature, const struct bitcoin_signature *htlc_signature, const struct added_htlc *added, const struct fulfilled_htlc *fulfilled, const struct failed_htlc **failed, const struct changed_htlc *changed, const struct bitcoin_tx *tx);
bool fromwire_channeld_got_commitsig(const tal_t *ctx, const void *p, u64 *commitnum, struct fee_states **fee_states, struct bitcoin_signature *signature, struct bitcoin_signature **htlc_signature, struct added_htlc **added, struct fulfilled_htlc **fulfilled, struct failed_htlc ***failed, struct changed_htlc **changed, struct bitcoin_tx **tx);

/* WIRE: CHANNELD_GOT_COMMITSIG_REPLY */
/*  Wait for reply */
u8 *towire_channeld_got_commitsig_reply(const tal_t *ctx);
bool fromwire_channeld_got_commitsig_reply(const void *p);

/* WIRE: CHANNELD_GOT_REVOKE */
u8 *towire_channeld_got_revoke(const tal_t *ctx, u64 revokenum, const struct secret *per_commitment_secret, const struct pubkey *next_per_commit_point, const struct fee_states *fee_states, const struct changed_htlc *changed, const struct penalty_base *pbase, const struct bitcoin_tx *penalty_tx);
bool fromwire_channeld_got_revoke(const tal_t *ctx, const void *p, u64 *revokenum, struct secret *per_commitment_secret, struct pubkey *next_per_commit_point, struct fee_states **fee_states, struct changed_htlc **changed, struct penalty_base **pbase, struct bitcoin_tx **penalty_tx);

/* WIRE: CHANNELD_GOT_REVOKE_REPLY */
/*  Wait for reply */
/*  (eg. if we sent another commitment_signed */
u8 *towire_channeld_got_revoke_reply(const tal_t *ctx);
bool fromwire_channeld_got_revoke_reply(const void *p);

/* WIRE: CHANNELD_SEND_SHUTDOWN */
/*  Tell peer to shut down channel. */
u8 *towire_channeld_send_shutdown(const tal_t *ctx, const u8 *shutdown_scriptpubkey);
bool fromwire_channeld_send_shutdown(const tal_t *ctx, const void *p, u8 **shutdown_scriptpubkey);

/* WIRE: CHANNELD_GOT_SHUTDOWN */
/*  Peer told us that channel is shutting down */
u8 *towire_channeld_got_shutdown(const tal_t *ctx, const u8 *scriptpubkey);
bool fromwire_channeld_got_shutdown(const tal_t *ctx, const void *p, u8 **scriptpubkey);

/* WIRE: CHANNELD_SHUTDOWN_COMPLETE */
/*  Shutdown is complete */
u8 *towire_channeld_shutdown_complete(const tal_t *ctx, const struct per_peer_state *per_peer_state);
bool fromwire_channeld_shutdown_complete(const tal_t *ctx, const void *p, struct per_peer_state **per_peer_state);

/* WIRE: CHANNELD_DEV_REENABLE_COMMIT */
/*  Re-enable commit timer. */
u8 *towire_channeld_dev_reenable_commit(const tal_t *ctx);
bool fromwire_channeld_dev_reenable_commit(const void *p);

/* WIRE: CHANNELD_DEV_REENABLE_COMMIT_REPLY */
u8 *towire_channeld_dev_reenable_commit_reply(const tal_t *ctx);
bool fromwire_channeld_dev_reenable_commit_reply(const void *p);

/* WIRE: CHANNELD_FEERATES */
u8 *towire_channeld_feerates(const tal_t *ctx, u32 feerate, u32 min_feerate, u32 max_feerate, u32 penalty_feerate);
bool fromwire_channeld_feerates(const void *p, u32 *feerate, u32 *min_feerate, u32 *max_feerate, u32 *penalty_feerate);

/* WIRE: CHANNELD_DEV_MEMLEAK */
/*  master -> channeld: do you have a memleak? */
u8 *towire_channeld_dev_memleak(const tal_t *ctx);
bool fromwire_channeld_dev_memleak(const void *p);

/* WIRE: CHANNELD_DEV_MEMLEAK_REPLY */
u8 *towire_channeld_dev_memleak_reply(const tal_t *ctx, bool leak);
bool fromwire_channeld_dev_memleak_reply(const void *p, bool *leak);

/* WIRE: CHANNELD_FAIL_FALLEN_BEHIND */
/*  Peer presented proof it was from the future. */
u8 *towire_channeld_fail_fallen_behind(const tal_t *ctx, const struct pubkey *remote_per_commitment_point);
bool fromwire_channeld_fail_fallen_behind(const tal_t *ctx, const void *p, struct pubkey **remote_per_commitment_point);

/* WIRE: CHANNELD_SPECIFIC_FEERATES */
/*  Handle a channel specific feerate base ppm configuration */
u8 *towire_channeld_specific_feerates(const tal_t *ctx, u32 feerate_base, u32 feerate_ppm);
bool fromwire_channeld_specific_feerates(const void *p, u32 *feerate_base, u32 *feerate_ppm);

/* WIRE: CHANNELD_GOT_ANNOUNCEMENT */
/*  When we receive announcement_signatures for channel announce */
u8 *towire_channeld_got_announcement(const tal_t *ctx, const secp256k1_ecdsa_signature *remote_ann_node_sig, const secp256k1_ecdsa_signature *remote_ann_bitcoin_sig);
bool fromwire_channeld_got_announcement(const void *p, secp256k1_ecdsa_signature *remote_ann_node_sig, secp256k1_ecdsa_signature *remote_ann_bitcoin_sig);

/* WIRE: CHANNELD_SEND_ERROR */
/*  Ask channeld to send a error message. Used in forgetting channel case. */
u8 *towire_channeld_send_error(const tal_t *ctx, const wirestring *reason);
bool fromwire_channeld_send_error(const tal_t *ctx, const void *p, wirestring **reason);

/* WIRE: CHANNELD_SEND_ERROR_REPLY */
/*  Tell master channeld has sent the error message. */
u8 *towire_channeld_send_error_reply(const tal_t *ctx);
bool fromwire_channeld_send_error_reply(const void *p);

/* WIRE: GOT_ONIONMSG_TO_US */
/*  Tell lightningd we got a onion message (for us */
u8 *towire_got_onionmsg_to_us(const tal_t *ctx, const struct pubkey *reply_blinding, const struct onionmsg_path **reply_path);
bool fromwire_got_onionmsg_to_us(const tal_t *ctx, const void *p, struct pubkey **reply_blinding, struct onionmsg_path ***reply_path);

/* WIRE: GOT_ONIONMSG_FORWARD */
u8 *towire_got_onionmsg_forward(const tal_t *ctx, const struct short_channel_id *next_scid, const struct node_id *next_node_id, const struct pubkey *next_blinding, const u8 next_onion[1366]);
bool fromwire_got_onionmsg_forward(const tal_t *ctx, const void *p, struct short_channel_id **next_scid, struct node_id **next_node_id, struct pubkey **next_blinding, u8 next_onion[1366]);

/* WIRE: SEND_ONIONMSG */
/*  Lightningd tells us to send a onion message. */
u8 *towire_send_onionmsg(const tal_t *ctx, const u8 onion[1366], const struct pubkey *blinding);
bool fromwire_send_onionmsg(const tal_t *ctx, const void *p, u8 onion[1366], struct pubkey **blinding);


#endif /* LIGHTNING_CHANNELD_CHANNELD_WIREGEN_H */

// SHA256STAMP:exp-0-1b6f8d6af6aeb028ca014ecd646ecff1fc72b6b47f4ae04a0d6b24b92efb6eda
