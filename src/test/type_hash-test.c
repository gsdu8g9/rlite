#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "../rlite.h"
#include "../type_hash.h"
#include "test_util.h"

#define UNSIGN(str) ((unsigned char *)(str))

static int basic_test_hset_hget(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_hset_hget %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);
	unsigned char *data2 = NULL;
	long data2len;

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, NULL, 0);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hget, RL_FOUND, db, key, keylen, field, fieldlen, &data2, &data2len);

	if (datalen != data2len) {
		fprintf(stderr, "expected %ld == %ld on line %d\n", datalen, data2len, __LINE__);
		retval = 1;
		goto cleanup;
	}

	if (memcmp(data, data2, datalen)) {
		fprintf(stderr, "expected %s == %s on line %d\n", data, data2, __LINE__);
		retval = 1;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_hset_hget\n");
	retval = 0;
cleanup:
	rl_free(data2);
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int basic_test_hset_hexists(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_hset_hexists %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *field2 = UNSIGN("my field2");
	long field2len = strlen((char *)field2);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, NULL, 0);

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hexists, RL_FOUND, db, key, keylen, field, fieldlen);
	RL_CALL_VERBOSE(rl_hexists, RL_NOT_FOUND, db, key, keylen, field2, field2len);


	fprintf(stderr, "End basic_test_hset_hexists\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int basic_test_hset_hdel(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_hset_hdel %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *field2 = UNSIGN("my field2");
	long field2len = strlen((char *)field2);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, NULL, 0);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field2, field2len, data, datalen, NULL, 0);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	long deleted;
	unsigned char *fields[3] = {field, field2, data};
	long fieldslen[3] = {fieldlen, field2len, datalen};
	RL_CALL_VERBOSE(rl_hdel, RL_OK, db, key, keylen, 3, fields, fieldslen, &deleted);

	if (deleted != 2) {
		fprintf(stderr, "Expected to delete 2 fields, got %ld instead on line %d\n", deleted, __LINE__);
		return 1;
	}

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_key_get, RL_NOT_FOUND, db, key, keylen, NULL, NULL, NULL, NULL);

	fprintf(stderr, "End basic_test_hset_hdel\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int basic_test_hset_hgetall(int _commit)
{
	int retval = 0;
	fprintf(stderr, "Start basic_test_hset_hgetall %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *field2 = UNSIGN("my field2");
	long field2len = strlen((char *)field2);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);
	unsigned char *data2 = UNSIGN("my data2");
	long data2len = strlen((char *)data2);

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, NULL, 0);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field2, field2len, data2, data2len, NULL, 0);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	rl_hash_iterator *iterator;
	unsigned char *f, *m;
	long fl, ml, i = 0;
	RL_CALL_VERBOSE(rl_hgetall, RL_OK, db, &iterator, key, keylen);
	while ((retval = rl_hash_iterator_next(iterator, &f, &fl, &m, &ml)) == RL_OK) {
		if (i == 0) {
			if (fl != fieldlen || memcmp(f, field, fl) != 0) {
				fprintf(stderr, "Expected field to be \"%s\", got \"%s\" instead on line %d\n", field, f, __LINE__);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
			if (ml != datalen || memcmp(m, data, ml) != 0) {
				fprintf(stderr, "Expected field data to be \"%s\", got \"%s\" instead on line %d\n", data, m, __LINE__);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
		}
		else if (i == 0) {
			if (fl != field2len || memcmp(f, field2, fl) != 0) {
				fprintf(stderr, "Expected field to be \"%s\", got \"%s\" instead on line %d\n", field2, f, __LINE__);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
			if (ml != data2len || memcmp(m, data2, ml) != 0) {
				fprintf(stderr, "Expected field data to be \"%s\", got \"%s\" instead on line %d\n", data2, m, __LINE__);
				retval = RL_UNEXPECTED;
				goto cleanup;
			}
		}
		rl_free(f);
		rl_free(m);
		i++;
	}

	if (retval != RL_END) {
		fprintf(stderr, "Expected to end iterator on line %d\n", __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	if (i != 2) {
		fprintf(stderr, "Expected to end iterator with %ld iterations on line %d\n", i, __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_hset_hgetall\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int basic_test_hset_hlen(int _commit)
{
	int retval = 0;
	long len;
	fprintf(stderr, "Start basic_test_hset_hlen %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *field2 = UNSIGN("my field2");
	long field2len = strlen((char *)field2);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);
	unsigned char *data2 = UNSIGN("my data2");
	long data2len = strlen((char *)data2);

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, NULL, 0);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field2, field2len, data2, data2len, NULL, 0);
	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hlen, RL_OK, db, key, keylen, &len);

	if (len != 2) {
		fprintf(stderr, "Expected len to be %ld on line %d\n", len, __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_hset_hlen\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int basic_test_hsetnx(int _commit)
{
	int retval = 0;
	long added;
	fprintf(stderr, "Start basic_test_hsetnx %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);
	unsigned char *data2 = UNSIGN("my data2");
	long data2len = strlen((char *)data2);
	unsigned char *data3;
	long data3len;

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, &added, 0);

	if (added != 1) {
		fprintf(stderr, "Expected added to be 1 on line %d\n", __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hset, RL_FOUND, db, key, keylen, field, fieldlen, data2, data2len, &added, 0);

	if (added != 0) {
		fprintf(stderr, "Expected added to be 0 on line %d\n", __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	RL_CALL_VERBOSE(rl_hget, RL_FOUND, db, key, keylen, field, fieldlen, &data3, &data3len);

	if (datalen != data3len) {
		fprintf(stderr, "expected %ld == %ld on line %d\n", datalen, data3len, __LINE__);
		retval = 1;
		goto cleanup;
	}

	if (memcmp(data, data3, datalen)) {
		fprintf(stderr, "expected %s == %s on line %d\n", data, data3, __LINE__);
		retval = 1;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data2, data2len, &added, 1);

	if (added != 0) {
		fprintf(stderr, "Expected added to be 0 on line %d\n", __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	RL_CALL_VERBOSE(rl_hget, RL_FOUND, db, key, keylen, field, fieldlen, &data3, &data3len);

	if (data2len != data3len) {
		fprintf(stderr, "expected %ld == %ld on line %d\n", data2len, data3len, __LINE__);
		retval = 1;
		goto cleanup;
	}

	if (memcmp(data2, data3, data2len)) {
		fprintf(stderr, "expected %s == %s on line %d\n", data2, data3, __LINE__);
		retval = 1;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_hsetnx\n");
	retval = 0;
cleanup:
	if (db) {
		rl_close(db);
	}
	return retval;
}

static int basic_test_hset_hmget(int _commit)
{
	int retval = 0;
	long added;
	fprintf(stderr, "Start basic_test_hset_hmget %d\n", _commit);

	rlite *db = NULL;
	RL_CALL_VERBOSE(setup_db, RL_OK, &db, _commit, 1);
	unsigned char *key = UNSIGN("my key");
	long keylen = strlen((char *)key);
	unsigned char *field = UNSIGN("my field");
	long fieldlen = strlen((char *)field);
	unsigned char *field2 = UNSIGN("my field2");
	long field2len = strlen((char *)field2);
	unsigned char *data = UNSIGN("my data");
	long datalen = strlen((char *)data);
	unsigned char *data2 = UNSIGN("my data2");
	long data2len = strlen((char *)data2);
	unsigned char *fields[3] = {field, (unsigned char *)"nonexistent", field2};
	long fieldslen[3] = {fieldlen, strlen((char *)fields[1]), field2len};
	unsigned char **datas = NULL;
	long *dataslen = NULL;

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field, fieldlen, data, datalen, &added, 0);

	if (added != 1) {
		fprintf(stderr, "Expected added to be 1 on line %d\n", __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hset, RL_OK, db, key, keylen, field2, field2len, data2, data2len, &added, 0);

	if (added != 1) {
		fprintf(stderr, "Expected added to be 1 on line %d\n", __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);

	if (_commit) {
		RL_CALL_VERBOSE(rl_commit, RL_OK, db);
		RL_CALL_VERBOSE(rl_is_balanced, RL_OK, db);
	}

	RL_CALL_VERBOSE(rl_hmget, RL_FOUND, db, key, keylen, 3, fields, fieldslen, &datas, &dataslen);

	if (dataslen[0] != datalen || memcmp(datas[0], data, datalen)) {
		fprintf(stderr, "Expected datas[0] \"%s\" to match data \"%s\" on line %d", datas[0], data, __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	if (dataslen[1] != -1 || datas[1] != NULL) {
		fprintf(stderr, "Expected datas[1] \"%s\" to be null on line %d", datas[1], __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	if (dataslen[2] != data2len || memcmp(datas[2], data2, data2len)) {
		fprintf(stderr, "Expected datas[2] \"%s\" to match data2 \"%s\" on line %d", datas[2], data2, __LINE__);
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	fprintf(stderr, "End basic_test_hset_hmget\n");
	retval = 0;
cleanup:
	if (datas) {
		for (added = 0; added < 3; added++) {
			rl_free(datas[added]);
		}
		rl_free(datas);
	}
	rl_free(dataslen);
	if (db) {
		rl_close(db);
	}
	return retval;
}

RL_TEST_MAIN_START(type_hash_test)
{
	int i;
	for (i = 0; i < 2; i++) {
		RL_TEST(basic_test_hset_hget, i);
		RL_TEST(basic_test_hset_hexists, i);
		RL_TEST(basic_test_hset_hdel, i);
		RL_TEST(basic_test_hset_hgetall, i);
		RL_TEST(basic_test_hset_hlen, i);
		RL_TEST(basic_test_hsetnx, i);
		RL_TEST(basic_test_hset_hmget, i);
	}
}
RL_TEST_MAIN_END
