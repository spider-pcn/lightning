struct {
	enum channel_add_err v;
	const char *name;
} enum_channel_add_err_names[] = {
	{ CHANNEL_ERR_ADD_OK, "CHANNEL_ERR_ADD_OK" },
	{ CHANNEL_ERR_INVALID_EXPIRY, "CHANNEL_ERR_INVALID_EXPIRY" },
	{ CHANNEL_ERR_DUPLICATE, "CHANNEL_ERR_DUPLICATE" },
	{ CHANNEL_ERR_DUPLICATE_ID_DIFFERENT, "CHANNEL_ERR_DUPLICATE_ID_DIFFERENT" },
	{ CHANNEL_ERR_MAX_HTLC_VALUE_EXCEEDED, "CHANNEL_ERR_MAX_HTLC_VALUE_EXCEEDED" },
	{ CHANNEL_ERR_CHANNEL_CAPACITY_EXCEEDED, "CHANNEL_ERR_CHANNEL_CAPACITY_EXCEEDED" },
	{ CHANNEL_ERR_HTLC_BELOW_MINIMUM, "CHANNEL_ERR_HTLC_BELOW_MINIMUM" },
	{ CHANNEL_ERR_TOO_MANY_HTLCS, "CHANNEL_ERR_TOO_MANY_HTLCS" },
	{ 0, NULL } };
struct {
	enum channel_remove_err v;
	const char *name;
} enum_channel_remove_err_names[] = {
	{ CHANNEL_ERR_REMOVE_OK, "CHANNEL_ERR_REMOVE_OK" },
	{ CHANNEL_ERR_NO_SUCH_ID, "CHANNEL_ERR_NO_SUCH_ID" },
	{ CHANNEL_ERR_ALREADY_FULFILLED, "CHANNEL_ERR_ALREADY_FULFILLED" },
	{ CHANNEL_ERR_BAD_PREIMAGE, "CHANNEL_ERR_BAD_PREIMAGE" },
	{ CHANNEL_ERR_HTLC_UNCOMMITTED, "CHANNEL_ERR_HTLC_UNCOMMITTED" },
	{ CHANNEL_ERR_HTLC_NOT_IRREVOCABLE, "CHANNEL_ERR_HTLC_NOT_IRREVOCABLE" },
	{ 0, NULL } };
