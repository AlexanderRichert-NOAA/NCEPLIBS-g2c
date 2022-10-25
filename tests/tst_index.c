/* This is a test for the NCEPLIBS-g2c project. 
 *
 * This test is for the g2c_write_index()/g2c_read_index() functions,
 * which write and read and index file that contians the byte offsets
 * to various parts of a GRIB2 file.
 *
 * Ed Hartnett 10/13/22
 */
#include "grib2_int.h"

#define FILE_NAME "tst_index.txt"
#define WAVE_FILE "gdaswave.t00z.wcoast.0p16.f000.grib2"
#define REF_FILE "ref_gdaswave.t00z.wcoast.0p16.f000.grb2index"
#define DEGRIB2_FILE "gdaswave.t00z.wcoast.0p16.f000.degrib2"
#define FTP_FILE "WW3_Regional_US_West_Coast_20220718_0000.grib2"
#define REF_FTP_FILE "ref_WW3_Regional_US_West_Coast_20220718_0000.grb2index"

int
main()
{
    printf("Testing g2c index functions.\n");
#ifdef JPEG
    printf("Testing g2c_read_index() on file %s...", WAVE_FILE);
    {
	int g2cid;
        int num_msg;
	int ret;

	/* Will not work. */
	if (g2c_read_index(NULL, NULL, 0, &g2cid) != G2C_EINVAL)
	    return G2C_ERROR;
	if (g2c_read_index("bad", NULL, 0, &g2cid) != G2C_EINVAL)
	    return G2C_ERROR;
	if (g2c_read_index("bad", "bad", 0, &g2cid) != G2C_EFILE)
	    return G2C_ERROR;

	/* Open the data file using the index file. */
	if ((ret = g2c_read_index(WAVE_FILE, REF_FILE, 0, &g2cid)))
	    return ret;

        /* Check some stuff. */
        if ((ret = g2c_inq(g2cid, &num_msg)))
            return ret;
        if (num_msg != 19)
            return G2C_ERROR;

        /* Output a degrib2 file. See tst_degrib2.c for a version of
         * this test that also checks the degrib2 output for
         * correctness. For this test, we just ensure it runs without
         * error. */
        if ((ret = g2c_degrib2(g2cid, DEGRIB2_FILE)))
            return ret;

        /* Close the file. */
	if ((ret = g2c_close(g2cid)))
	    return ret;
    }
    printf("ok!\n");
#ifdef FTP_TEST_FILES
    printf("Testing g2c_read_index() on file %s downloaded via FTP...", FTP_FILE);
    {
	int g2cid;
        int num_msg;
        unsigned char sig_ref_time, month, day, hour, minute, second;
        short year;
        short center, subcenter;
        unsigned char master_version, local_version;
        int num_fields, num_local;
        unsigned char discipline;
	int ret;

	/* g2c_set_log_level(11); */
	/* Open the data file using the index file. */
	if ((ret = g2c_read_index(FTP_FILE, REF_FTP_FILE, 0, &g2cid)))
	    return ret;

        /* Check some stuff. */
        if ((ret = g2c_inq(g2cid, &num_msg)))
            return ret;
        if (num_msg != 688)
            return G2C_ERROR;

        /* Check some stuff about the last message. */
        if ((ret = g2c_inq_msg(g2cid, 687, &discipline, &num_fields, &num_local,
                               &center, &subcenter, &master_version, &local_version)))
            return ret;
        if (center != 7 || subcenter != 0 || master_version != 2 || local_version != 1)
            return G2C_ERROR;
        if (num_local || num_fields != 1 || discipline != 10)
            return G2C_ERROR;

        /* Inquire about the date/time. */
        if ((ret = g2c_inq_msg_time(g2cid, 687, &sig_ref_time, &year, &month, &day, &hour,
                                    &minute, &second)))
            return ret;

        /* Check date/time. */
        if (sig_ref_time != 1 || year != 2022 || month != 7 || day != 18 ||
            hour != 0 || minute != 0 || second != 0)
            return G2C_ERROR;

        /* Close the file. */
	if ((ret = g2c_close(g2cid)))
	    return ret;
    }
    printf("ok!\n");
#endif /* FTP_TEST_FILES */
#endif /* JPEG */
    printf("SUCCESS!\n");
    return 0;
}
