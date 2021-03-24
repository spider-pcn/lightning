/* This file was generated by generate-wire.py */
/* Do not modify this file! Modify the _csv file it was generated from. */
/* Original template can be found at tools/gen/header_template */

#ifndef LIGHTNING_HSMD_GEN_HSM_WIRE_H
#define LIGHTNING_HSMD_GEN_HSM_WIRE_H
#include <ccan/tal/tal.h>
#include <wire/tlvstream.h>
#include <wire/wire.h>
#include <bitcoin/chainparams.h>
#include <common/bip32.h>
#include <common/derive_basepoints.h>
#include <common/utxo.h>

enum hsm_wire_type {
        /*  Clients should not give a bad request but not the HSM's decision to crash. */
        WIRE_HSMSTATUS_CLIENT_BAD_REQUEST = 1000,
        /*  Start the HSM. */
        WIRE_HSM_INIT = 11,
        WIRE_HSM_INIT_REPLY = 111,
        /*  Get a new HSM FD */
        WIRE_HSM_CLIENT_HSMFD = 9,
        /*  No content */
        WIRE_HSM_CLIENT_HSMFD_REPLY = 109,
        /*  Get the basepoints and funding key for this specific channel. */
        WIRE_HSM_GET_CHANNEL_BASEPOINTS = 10,
        WIRE_HSM_GET_CHANNEL_BASEPOINTS_REPLY = 110,
        /*  Return signature for a funding tx. */
        /*  FIXME: This should also take their commit sig & details */
        WIRE_HSM_SIGN_FUNDING = 4,
        WIRE_HSM_SIGN_FUNDING_REPLY = 104,
        /*  Master asks the HSM to sign a node_announcement */
        WIRE_HSM_NODE_ANNOUNCEMENT_SIG_REQ = 6,
        WIRE_HSM_NODE_ANNOUNCEMENT_SIG_REPLY = 106,
        /*  Sign a withdrawal request */
        WIRE_HSM_SIGN_WITHDRAWAL = 7,
        WIRE_HSM_SIGN_WITHDRAWAL_REPLY = 107,
        /*  Sign an invoice */
        WIRE_HSM_SIGN_INVOICE = 8,
        WIRE_HSM_SIGN_INVOICE_REPLY = 108,
        /*  Give me ECDH(node-id-secret */
        WIRE_HSM_ECDH_REQ = 1,
        WIRE_HSM_ECDH_RESP = 100,
        WIRE_HSM_CANNOUNCEMENT_SIG_REQ = 2,
        WIRE_HSM_CANNOUNCEMENT_SIG_REPLY = 102,
        WIRE_HSM_CUPDATE_SIG_REQ = 3,
        WIRE_HSM_CUPDATE_SIG_REPLY = 103,
        /*  Master asks HSM to sign a commitment transaction. */
        WIRE_HSM_SIGN_COMMITMENT_TX = 5,
        WIRE_HSM_SIGN_COMMITMENT_TX_REPLY = 105,
        /*  Onchaind asks HSM to sign a spend to-us.  Four variants */
        /*  of keys is derived differently... */
        /*  FIXME: Have master tell hsmd the keyindex */
        WIRE_HSM_SIGN_DELAYED_PAYMENT_TO_US = 12,
        WIRE_HSM_SIGN_REMOTE_HTLC_TO_US = 13,
        WIRE_HSM_SIGN_PENALTY_TO_US = 14,
        /*  Onchaind asks HSM to sign a local HTLC success or HTLC timeout tx. */
        WIRE_HSM_SIGN_LOCAL_HTLC_TX = 16,
        /*  Openingd/channeld asks HSM to sign the other sides' commitment tx. */
        WIRE_HSM_SIGN_REMOTE_COMMITMENT_TX = 19,
        /*  channeld asks HSM to sign remote HTLC tx. */
        WIRE_HSM_SIGN_REMOTE_HTLC_TX = 20,
        /*  closingd asks HSM to sign mutual close tx. */
        WIRE_HSM_SIGN_MUTUAL_CLOSE_TX = 21,
        /*  Reply for all the above requests. */
        WIRE_HSM_SIGN_TX_REPLY = 112,
        /*  Openingd/channeld/onchaind asks for Nth per_commitment_point */
        WIRE_HSM_GET_PER_COMMITMENT_POINT = 18,
        WIRE_HSM_GET_PER_COMMITMENT_POINT_REPLY = 118,
        /*  master -> hsmd: do you have a memleak? */
        WIRE_HSM_DEV_MEMLEAK = 33,
        WIRE_HSM_DEV_MEMLEAK_REPLY = 133,
        /*  channeld asks to check if claimed future commitment_secret is correct. */
        WIRE_HSM_CHECK_FUTURE_SECRET = 22,
        WIRE_HSM_CHECK_FUTURE_SECRET_REPLY = 122,
        /*  lightningd asks us to sign a string. */
        WIRE_HSM_SIGN_MESSAGE = 23,
        WIRE_HSM_SIGN_MESSAGE_REPLY = 123,
};

const char *hsm_wire_type_name(int e);


/* WIRE: HSMSTATUS_CLIENT_BAD_REQUEST */
/*  Clients should not give a bad request but not the HSM's decision to crash. */
u8 *towire_hsmstatus_client_bad_request(const tal_t *ctx, const struct node_id *id, const wirestring *description, const u8 *msg);
bool fromwire_hsmstatus_client_bad_request(const tal_t *ctx, const void *p, struct node_id *id, wirestring **description, u8 **msg);

/* WIRE: HSM_INIT */
/*  Start the HSM. */
u8 *towire_hsm_init(const tal_t *ctx, const struct bip32_key_version *bip32_key_version, const struct chainparams *chainparams, const struct secret *hsm_encryption_key, const struct privkey *dev_force_privkey, const struct secret *dev_force_bip32_seed, const struct secrets *dev_force_channel_secrets, const struct sha256 *dev_force_channel_secrets_shaseed);
bool fromwire_hsm_init(const tal_t *ctx, const void *p, struct bip32_key_version *bip32_key_version, const struct chainparams **chainparams, struct secret **hsm_encryption_key, struct privkey **dev_force_privkey, struct secret **dev_force_bip32_seed, struct secrets **dev_force_channel_secrets, struct sha256 **dev_force_channel_secrets_shaseed);

/* WIRE: HSM_INIT_REPLY */
u8 *towire_hsm_init_reply(const tal_t *ctx, const struct node_id *node_id, const struct ext_key *bip32);
bool fromwire_hsm_init_reply(const void *p, struct node_id *node_id, struct ext_key *bip32);

/* WIRE: HSM_CLIENT_HSMFD */
/*  Get a new HSM FD */
u8 *towire_hsm_client_hsmfd(const tal_t *ctx, const struct node_id *id, u64 dbid, u64 capabilities);
bool fromwire_hsm_client_hsmfd(const void *p, struct node_id *id, u64 *dbid, u64 *capabilities);

/* WIRE: HSM_CLIENT_HSMFD_REPLY */
/*  No content */
u8 *towire_hsm_client_hsmfd_reply(const tal_t *ctx);
bool fromwire_hsm_client_hsmfd_reply(const void *p);

/* WIRE: HSM_GET_CHANNEL_BASEPOINTS */
/*  Get the basepoints and funding key for this specific channel. */
u8 *towire_hsm_get_channel_basepoints(const tal_t *ctx, const struct node_id *peerid, u64 dbid);
bool fromwire_hsm_get_channel_basepoints(const void *p, struct node_id *peerid, u64 *dbid);

/* WIRE: HSM_GET_CHANNEL_BASEPOINTS_REPLY */
u8 *towire_hsm_get_channel_basepoints_reply(const tal_t *ctx, const struct basepoints *basepoints, const struct pubkey *funding_pubkey);
bool fromwire_hsm_get_channel_basepoints_reply(const void *p, struct basepoints *basepoints, struct pubkey *funding_pubkey);

/* WIRE: HSM_SIGN_FUNDING */
/*  Return signature for a funding tx. */
/*  FIXME: This should also take their commit sig & details */
u8 *towire_hsm_sign_funding(const tal_t *ctx, struct amount_sat satoshi_out, struct amount_sat change_out, u32 change_keyindex, const struct pubkey *our_pubkey, const struct pubkey *their_pubkey, const struct utxo **inputs);
bool fromwire_hsm_sign_funding(const tal_t *ctx, const void *p, struct amount_sat *satoshi_out, struct amount_sat *change_out, u32 *change_keyindex, struct pubkey *our_pubkey, struct pubkey *their_pubkey, struct utxo ***inputs);

/* WIRE: HSM_SIGN_FUNDING_REPLY */
u8 *towire_hsm_sign_funding_reply(const tal_t *ctx, const struct bitcoin_tx *tx);
bool fromwire_hsm_sign_funding_reply(const tal_t *ctx, const void *p, struct bitcoin_tx **tx);

/* WIRE: HSM_NODE_ANNOUNCEMENT_SIG_REQ */
/*  Master asks the HSM to sign a node_announcement */
u8 *towire_hsm_node_announcement_sig_req(const tal_t *ctx, const u8 *announcement);
bool fromwire_hsm_node_announcement_sig_req(const tal_t *ctx, const void *p, u8 **announcement);

/* WIRE: HSM_NODE_ANNOUNCEMENT_SIG_REPLY */
u8 *towire_hsm_node_announcement_sig_reply(const tal_t *ctx, const secp256k1_ecdsa_signature *signature);
bool fromwire_hsm_node_announcement_sig_reply(const void *p, secp256k1_ecdsa_signature *signature);

/* WIRE: HSM_SIGN_WITHDRAWAL */
/*  Sign a withdrawal request */
u8 *towire_hsm_sign_withdrawal(const tal_t *ctx, struct amount_sat satoshi_out, struct amount_sat change_out, u32 change_keyindex, const struct bitcoin_tx_output **outputs, const struct utxo **inputs);
bool fromwire_hsm_sign_withdrawal(const tal_t *ctx, const void *p, struct amount_sat *satoshi_out, struct amount_sat *change_out, u32 *change_keyindex, struct bitcoin_tx_output ***outputs, struct utxo ***inputs);

/* WIRE: HSM_SIGN_WITHDRAWAL_REPLY */
u8 *towire_hsm_sign_withdrawal_reply(const tal_t *ctx, const struct bitcoin_tx *tx);
bool fromwire_hsm_sign_withdrawal_reply(const tal_t *ctx, const void *p, struct bitcoin_tx **tx);

/* WIRE: HSM_SIGN_INVOICE */
/*  Sign an invoice */
u8 *towire_hsm_sign_invoice(const tal_t *ctx, const u8 *u5bytes, const u8 *hrp);
bool fromwire_hsm_sign_invoice(const tal_t *ctx, const void *p, u8 **u5bytes, u8 **hrp);

/* WIRE: HSM_SIGN_INVOICE_REPLY */
u8 *towire_hsm_sign_invoice_reply(const tal_t *ctx, const secp256k1_ecdsa_recoverable_signature *sig);
bool fromwire_hsm_sign_invoice_reply(const void *p, secp256k1_ecdsa_recoverable_signature *sig);

/* WIRE: HSM_ECDH_REQ */
/*  Give me ECDH(node-id-secret */
u8 *towire_hsm_ecdh_req(const tal_t *ctx, const struct pubkey *point);
bool fromwire_hsm_ecdh_req(const void *p, struct pubkey *point);

/* WIRE: HSM_ECDH_RESP */
u8 *towire_hsm_ecdh_resp(const tal_t *ctx, const struct secret *ss);
bool fromwire_hsm_ecdh_resp(const void *p, struct secret *ss);

/* WIRE: HSM_CANNOUNCEMENT_SIG_REQ */
u8 *towire_hsm_cannouncement_sig_req(const tal_t *ctx, const u8 *ca);
bool fromwire_hsm_cannouncement_sig_req(const tal_t *ctx, const void *p, u8 **ca);

/* WIRE: HSM_CANNOUNCEMENT_SIG_REPLY */
u8 *towire_hsm_cannouncement_sig_reply(const tal_t *ctx, const secp256k1_ecdsa_signature *node_signature, const secp256k1_ecdsa_signature *bitcoin_signature);
bool fromwire_hsm_cannouncement_sig_reply(const void *p, secp256k1_ecdsa_signature *node_signature, secp256k1_ecdsa_signature *bitcoin_signature);

/* WIRE: HSM_CUPDATE_SIG_REQ */
u8 *towire_hsm_cupdate_sig_req(const tal_t *ctx, const u8 *cu);
bool fromwire_hsm_cupdate_sig_req(const tal_t *ctx, const void *p, u8 **cu);

/* WIRE: HSM_CUPDATE_SIG_REPLY */
u8 *towire_hsm_cupdate_sig_reply(const tal_t *ctx, const u8 *cu);
bool fromwire_hsm_cupdate_sig_reply(const tal_t *ctx, const void *p, u8 **cu);

/* WIRE: HSM_SIGN_COMMITMENT_TX */
/*  Master asks HSM to sign a commitment transaction. */
u8 *towire_hsm_sign_commitment_tx(const tal_t *ctx, const struct node_id *peer_id, u64 channel_dbid, const struct bitcoin_tx *tx, const struct pubkey *remote_funding_key, struct amount_sat funding_amount);
bool fromwire_hsm_sign_commitment_tx(const tal_t *ctx, const void *p, struct node_id *peer_id, u64 *channel_dbid, struct bitcoin_tx **tx, struct pubkey *remote_funding_key, struct amount_sat *funding_amount);

/* WIRE: HSM_SIGN_COMMITMENT_TX_REPLY */
u8 *towire_hsm_sign_commitment_tx_reply(const tal_t *ctx, const struct bitcoin_signature *sig);
bool fromwire_hsm_sign_commitment_tx_reply(const void *p, struct bitcoin_signature *sig);

/* WIRE: HSM_SIGN_DELAYED_PAYMENT_TO_US */
/*  Onchaind asks HSM to sign a spend to-us.  Four variants */
/*  of keys is derived differently... */
/*  FIXME: Have master tell hsmd the keyindex */
u8 *towire_hsm_sign_delayed_payment_to_us(const tal_t *ctx, u64 commit_num, const struct bitcoin_tx *tx, const u8 *wscript, struct amount_sat input_amount);
bool fromwire_hsm_sign_delayed_payment_to_us(const tal_t *ctx, const void *p, u64 *commit_num, struct bitcoin_tx **tx, u8 **wscript, struct amount_sat *input_amount);

/* WIRE: HSM_SIGN_REMOTE_HTLC_TO_US */
u8 *towire_hsm_sign_remote_htlc_to_us(const tal_t *ctx, const struct pubkey *remote_per_commitment_point, const struct bitcoin_tx *tx, const u8 *wscript, struct amount_sat input_amount);
bool fromwire_hsm_sign_remote_htlc_to_us(const tal_t *ctx, const void *p, struct pubkey *remote_per_commitment_point, struct bitcoin_tx **tx, u8 **wscript, struct amount_sat *input_amount);

/* WIRE: HSM_SIGN_PENALTY_TO_US */
u8 *towire_hsm_sign_penalty_to_us(const tal_t *ctx, const struct secret *revocation_secret, const struct bitcoin_tx *tx, const u8 *wscript, struct amount_sat input_amount);
bool fromwire_hsm_sign_penalty_to_us(const tal_t *ctx, const void *p, struct secret *revocation_secret, struct bitcoin_tx **tx, u8 **wscript, struct amount_sat *input_amount);

/* WIRE: HSM_SIGN_LOCAL_HTLC_TX */
/*  Onchaind asks HSM to sign a local HTLC success or HTLC timeout tx. */
u8 *towire_hsm_sign_local_htlc_tx(const tal_t *ctx, u64 commit_num, const struct bitcoin_tx *tx, const u8 *wscript, struct amount_sat input_amount);
bool fromwire_hsm_sign_local_htlc_tx(const tal_t *ctx, const void *p, u64 *commit_num, struct bitcoin_tx **tx, u8 **wscript, struct amount_sat *input_amount);

/* WIRE: HSM_SIGN_REMOTE_COMMITMENT_TX */
/*  Openingd/channeld asks HSM to sign the other sides' commitment tx. */
u8 *towire_hsm_sign_remote_commitment_tx(const tal_t *ctx, const struct bitcoin_tx *tx, const struct pubkey *remote_funding_key, struct amount_sat funding_amount);
bool fromwire_hsm_sign_remote_commitment_tx(const tal_t *ctx, const void *p, struct bitcoin_tx **tx, struct pubkey *remote_funding_key, struct amount_sat *funding_amount);

/* WIRE: HSM_SIGN_REMOTE_HTLC_TX */
/*  channeld asks HSM to sign remote HTLC tx. */
u8 *towire_hsm_sign_remote_htlc_tx(const tal_t *ctx, const struct bitcoin_tx *tx, const u8 *wscript, struct amount_sat amounts_satoshi, const struct pubkey *remote_per_commit_point);
bool fromwire_hsm_sign_remote_htlc_tx(const tal_t *ctx, const void *p, struct bitcoin_tx **tx, u8 **wscript, struct amount_sat *amounts_satoshi, struct pubkey *remote_per_commit_point);

/* WIRE: HSM_SIGN_MUTUAL_CLOSE_TX */
/*  closingd asks HSM to sign mutual close tx. */
u8 *towire_hsm_sign_mutual_close_tx(const tal_t *ctx, const struct bitcoin_tx *tx, const struct pubkey *remote_funding_key, struct amount_sat funding);
bool fromwire_hsm_sign_mutual_close_tx(const tal_t *ctx, const void *p, struct bitcoin_tx **tx, struct pubkey *remote_funding_key, struct amount_sat *funding);

/* WIRE: HSM_SIGN_TX_REPLY */
/*  Reply for all the above requests. */
u8 *towire_hsm_sign_tx_reply(const tal_t *ctx, const struct bitcoin_signature *sig);
bool fromwire_hsm_sign_tx_reply(const void *p, struct bitcoin_signature *sig);

/* WIRE: HSM_GET_PER_COMMITMENT_POINT */
/*  Openingd/channeld/onchaind asks for Nth per_commitment_point */
u8 *towire_hsm_get_per_commitment_point(const tal_t *ctx, u64 n);
bool fromwire_hsm_get_per_commitment_point(const void *p, u64 *n);

/* WIRE: HSM_GET_PER_COMMITMENT_POINT_REPLY */
u8 *towire_hsm_get_per_commitment_point_reply(const tal_t *ctx, const struct pubkey *per_commitment_point, const struct secret *old_commitment_secret);
bool fromwire_hsm_get_per_commitment_point_reply(const tal_t *ctx, const void *p, struct pubkey *per_commitment_point, struct secret **old_commitment_secret);

/* WIRE: HSM_DEV_MEMLEAK */
/*  master -> hsmd: do you have a memleak? */
u8 *towire_hsm_dev_memleak(const tal_t *ctx);
bool fromwire_hsm_dev_memleak(const void *p);

/* WIRE: HSM_DEV_MEMLEAK_REPLY */
u8 *towire_hsm_dev_memleak_reply(const tal_t *ctx, bool leak);
bool fromwire_hsm_dev_memleak_reply(const void *p, bool *leak);

/* WIRE: HSM_CHECK_FUTURE_SECRET */
/*  channeld asks to check if claimed future commitment_secret is correct. */
u8 *towire_hsm_check_future_secret(const tal_t *ctx, u64 n, const struct secret *commitment_secret);
bool fromwire_hsm_check_future_secret(const void *p, u64 *n, struct secret *commitment_secret);

/* WIRE: HSM_CHECK_FUTURE_SECRET_REPLY */
u8 *towire_hsm_check_future_secret_reply(const tal_t *ctx, bool correct);
bool fromwire_hsm_check_future_secret_reply(const void *p, bool *correct);

/* WIRE: HSM_SIGN_MESSAGE */
/*  lightningd asks us to sign a string. */
u8 *towire_hsm_sign_message(const tal_t *ctx, const u8 *msg);
bool fromwire_hsm_sign_message(const tal_t *ctx, const void *p, u8 **msg);

/* WIRE: HSM_SIGN_MESSAGE_REPLY */
u8 *towire_hsm_sign_message_reply(const tal_t *ctx, const secp256k1_ecdsa_recoverable_signature *sig);
bool fromwire_hsm_sign_message_reply(const void *p, secp256k1_ecdsa_recoverable_signature *sig);


#endif /* LIGHTNING_HSMD_GEN_HSM_WIRE_H */

