// server/CMathTest.cpp
// server/CMathTest.cpp
// server/CMathTest.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/maths.h>
#include <util/hash.h>
#include <util/CUnitTest.h>

//#define NO_UNITTESTS
#ifndef NO_UNITTESTS

class CMathTest : public CUnitTest
{
public:
	CMathTest() : CUnitTest("MathTest") {}
	virtual bool RunTests( void );
private:
	bool testWrapDegrees();
};

CMathTest test;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CMathTest::RunTests()
{
	if( !testWrapDegrees() )
		return false;

	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CMathTest::testWrapDegrees()
{
	float tol = 0.001f;
	float d;
	d = wrapDegrees(25.0f);
	if( !FloatEqual(d, 25.0f, tol) )
		return false;

	d = wrapDegrees(370.0f);
	if( !FloatEqual(d, 10.0f, tol) )
		return false;

	d = wrapDegrees(730.0f);
	if( !FloatEqual(d, 10.0f, tol) )
		return false;

	d = wrapDegrees(-25.0f);
	if( !FloatEqual(d, 360.0f-25.0f, tol) )
		return false;

	d = wrapDegrees(-370.0f);
	if( !FloatEqual(d, 360.0f-10.0f, tol) )
		return false;

	d = wrapDegrees(-730.0f);
	if( !FloatEqual(d, 360.0f-10.0f, tol) )
		return false;

	return true;
}


//#############################################################
// HASH FUNCTION TESTS
//#############################################################

//TESTS FOLLOW, from http://www.isthe.com/chongo/src/fnv/test_fnv.c

#define LEN(x) (sizeof(x)-1)
/* TEST macro does not include trailing NUL byte in the test vector */
#define TEST(x) {x, LEN(x)}
/* TEST0 macro includes the trailing NUL byte in the test vector */
#define TEST0(x) {x, sizeof(x)}
/* REPEAT500 - repeat a string 500 times */
#define R500(x) R100(x)R100(x)R100(x)R100(x)R100(x)
#define R100(x) R10(x)R10(x)R10(x)R10(x)R10(x)R10(x)R10(x)R10(x)R10(x)R10(x)
#define R10(x) x x x x x x x x x x

typedef uint32_t Fnv32_t;

struct test_vector {
	const void *buf;		/* start of test vector buffer */
	size_t len;		/* length of test vector */
};

struct test_vector fnv_test_str[] = {
	TEST(""),
	TEST("a"),
	TEST("b"),
	TEST("c"),
	TEST("d"),
	TEST("e"),
	TEST("f"),
	TEST("fo"),
	TEST("foo"),
	TEST("foob"),
	TEST("fooba"),
	TEST("foobar"),
	TEST0(""),
	TEST0("a"),
	TEST0("b"),
	TEST0("c"),
	TEST0("d"),
	TEST0("e"),
	TEST0("f"),
	TEST0("fo"),
	TEST0("foo"),
	TEST0("foob"),
	TEST0("fooba"),
	TEST0("foobar"),
	TEST("ch"),
	TEST("cho"),
	TEST("chon"),
	TEST("chong"),
	TEST("chongo"),
	TEST("chongo "),
	TEST("chongo w"),
	TEST("chongo wa"),
	TEST("chongo was"),
	TEST("chongo was "),
	TEST("chongo was h"),
	TEST("chongo was he"),
	TEST("chongo was her"),
	TEST("chongo was here"),
	TEST("chongo was here!"),
	TEST("chongo was here!\n"),
	TEST0("ch"),
	TEST0("cho"),
	TEST0("chon"),
	TEST0("chong"),
	TEST0("chongo"),
	TEST0("chongo "),
	TEST0("chongo w"),
	TEST0("chongo wa"),
	TEST0("chongo was"),
	TEST0("chongo was "),
	TEST0("chongo was h"),
	TEST0("chongo was he"),
	TEST0("chongo was her"),
	TEST0("chongo was here"),
	TEST0("chongo was here!"),
	TEST0("chongo was here!\n"),
	TEST("cu"),
	TEST("cur"),
	TEST("curd"),
	TEST("curds"),
	TEST("curds "),
	TEST("curds a"),
	TEST("curds an"),
	TEST("curds and"),
	TEST("curds and "),
	TEST("curds and w"),
	TEST("curds and wh"),
	TEST("curds and whe"),
	TEST("curds and whey"),
	TEST("curds and whey\n"),
	TEST0("cu"),
	TEST0("cur"),
	TEST0("curd"),
	TEST0("curds"),
	TEST0("curds "),
	TEST0("curds a"),
	TEST0("curds an"),
	TEST0("curds and"),
	TEST0("curds and "),
	TEST0("curds and w"),
	TEST0("curds and wh"),
	TEST0("curds and whe"),
	TEST0("curds and whey"),
	TEST0("curds and whey\n"),
	TEST("hi"), TEST0("hi"),
	TEST("hello"), TEST0("hello"),
	TEST("\xff\x00\x00\x01"), TEST("\x01\x00\x00\xff"),
	TEST("\xff\x00\x00\x02"), TEST("\x02\x00\x00\xff"),
	TEST("\xff\x00\x00\x03"), TEST("\x03\x00\x00\xff"),
	TEST("\xff\x00\x00\x04"), TEST("\x04\x00\x00\xff"),
	TEST("\x40\x51\x4e\x44"), TEST("\x44\x4e\x51\x40"),
	TEST("\x40\x51\x4e\x4a"), TEST("\x4a\x4e\x51\x40"),
	TEST("\x40\x51\x4e\x54"), TEST("\x54\x4e\x51\x40"),
	TEST("127.0.0.1"), TEST0("127.0.0.1"),
	TEST("127.0.0.2"), TEST0("127.0.0.2"),
	TEST("127.0.0.3"), TEST0("127.0.0.3"),
	TEST("64.81.78.68"), TEST0("64.81.78.68"),
	TEST("64.81.78.74"), TEST0("64.81.78.74"),
	TEST("64.81.78.84"), TEST0("64.81.78.84"),
	TEST("feedface"), TEST0("feedface"),
	TEST("feedfacedaffdeed"), TEST0("feedfacedaffdeed"),
	TEST("feedfacedeadbeef"), TEST0("feedfacedeadbeef"),
	TEST("line 1\nline 2\nline 3"),
	TEST("chongo <Landon Curt Noll> /\\../\\"),
	TEST0("chongo <Landon Curt Noll> /\\../\\"),
	TEST("chongo (Landon Curt Noll) /\\../\\"),
	TEST0("chongo (Landon Curt Noll) /\\../\\"),
	TEST("http://antwrp.gsfc.nasa.gov/apod/astropix.html"),
	TEST("http://en.wikipedia.org/wiki/Fowler_Noll_Vo_hash"),
	TEST("http://epod.usra.edu/"),
	TEST("http://exoplanet.eu/"),
	TEST("http://hvo.wr.usgs.gov/cam3/"),
	TEST("http://hvo.wr.usgs.gov/cams/HMcam/"),
	TEST("http://hvo.wr.usgs.gov/kilauea/update/deformation.html"),
	TEST("http://hvo.wr.usgs.gov/kilauea/update/images.html"),
	TEST("http://hvo.wr.usgs.gov/kilauea/update/maps.html"),
	TEST("http://hvo.wr.usgs.gov/volcanowatch/current_issue.html"),
	TEST("http://neo.jpl.nasa.gov/risk/"),
	TEST("http://norvig.com/21-days.html"),
	TEST("http://primes.utm.edu/curios/home.php"),
	TEST("http://slashdot.org/"),
	TEST("http://tux.wr.usgs.gov/Maps/155.25-19.5.html"),
	TEST("http://volcano.wr.usgs.gov/kilaueastatus.php"),
	TEST("http://www.avo.alaska.edu/activity/Redoubt.php"),
	TEST("http://www.dilbert.com/fast/"),
	TEST("http://www.fourmilab.ch/gravitation/orbits/"),
	TEST("http://www.fpoa.net/"),
	TEST("http://www.ioccc.org/index.html"),
	TEST("http://www.isthe.com/cgi-bin/number.cgi"),
	TEST("http://www.isthe.com/chongo/bio.html"),
	TEST("http://www.isthe.com/chongo/index.html"),
	TEST("http://www.isthe.com/chongo/src/calc/lucas-calc"),
	TEST("http://www.isthe.com/chongo/tech/astro/venus2004.html"),
	TEST("http://www.isthe.com/chongo/tech/astro/vita.html"),
	TEST("http://www.isthe.com/chongo/tech/comp/c/expert.html"),
	TEST("http://www.isthe.com/chongo/tech/comp/calc/index.html"),
	TEST("http://www.isthe.com/chongo/tech/comp/fnv/index.html"),
	TEST("http://www.isthe.com/chongo/tech/math/number/howhigh.html"),
	TEST("http://www.isthe.com/chongo/tech/math/number/number.html"),
	TEST("http://www.isthe.com/chongo/tech/math/prime/mersenne.html"),
	TEST("http://www.isthe.com/chongo/tech/math/prime/mersenne.html#largest"),
	TEST("http://www.lavarnd.org/cgi-bin/corpspeak.cgi"),
	TEST("http://www.lavarnd.org/cgi-bin/haiku.cgi"),
	TEST("http://www.lavarnd.org/cgi-bin/rand-none.cgi"),
	TEST("http://www.lavarnd.org/cgi-bin/randdist.cgi"),
	TEST("http://www.lavarnd.org/index.html"),
	TEST("http://www.lavarnd.org/what/nist-test.html"),
	TEST("http://www.macosxhints.com/"),
	TEST("http://www.mellis.com/"),
	TEST("http://www.nature.nps.gov/air/webcams/parks/havoso2alert/havoalert.cfm"),
	TEST("http://www.nature.nps.gov/air/webcams/parks/havoso2alert/timelines_24.cfm"),
	TEST("http://www.paulnoll.com/"),
	TEST("http://www.pepysdiary.com/"),
	TEST("http://www.sciencenews.org/index/home/activity/view"),
	TEST("http://www.skyandtelescope.com/"),
	TEST("http://www.sput.nl/~rob/sirius.html"),
	TEST("http://www.systemexperts.com/"),
	TEST("http://www.tq-international.com/phpBB3/index.php"),
	TEST("http://www.travelquesttours.com/index.htm"),
	TEST("http://www.wunderground.com/global/stations/89606.html"),
	TEST(R10("21701")),
	TEST(R10("M21701")),
	TEST(R10("2^21701-1")),
	TEST(R10("\x54\xc5")),
	TEST(R10("\xc5\x54")),
	TEST(R10("23209")),
	TEST(R10("M23209")),
	TEST(R10("2^23209-1")),
	TEST(R10("\x5a\xa9")),
	TEST(R10("\xa9\x5a")),
	TEST(R10("391581216093")),
	TEST(R10("391581*2^216093-1")),
	TEST(R10("\x05\xf9\x9d\x03\x4c\x81")),
	TEST(R10("FEDCBA9876543210")),
	TEST(R10("\xfe\xdc\xba\x98\x76\x54\x32\x10")),
	TEST(R10("EFCDAB8967452301")),
	TEST(R10("\xef\xcd\xab\x89\x67\x45\x23\x01")),
	TEST(R10("0123456789ABCDEF")),
	TEST(R10("\x01\x23\x45\x67\x89\xab\xcd\xef")),
	TEST(R10("1032547698BADCFE")),
	TEST(R10("\x10\x32\x54\x76\x98\xba\xdc\xfe")),
	TEST(R500("\x00")),
	TEST(R500("\x07")),
	TEST(R500("~")),
	TEST(R500("\x7f")),
	{NULL, 0}	/* MUST BE LAST */
};

struct fnv1a_32_test_vector {
	struct test_vector *test;	/* test vector buffer to hash */
	Fnv32_t fnv1a_32;		/* expected FNV-1a 32 bit hash value */
};

/* FNV-1a 32 bit test vectors */
struct fnv1a_32_test_vector fnv1a_32_vector[] = {
	{ &fnv_test_str[0], (Fnv32_t) 0x811c9dc5UL },
	{ &fnv_test_str[1], (Fnv32_t) 0xe40c292cUL },
	{ &fnv_test_str[2], (Fnv32_t) 0xe70c2de5UL },
	{ &fnv_test_str[3], (Fnv32_t) 0xe60c2c52UL },
	{ &fnv_test_str[4], (Fnv32_t) 0xe10c2473UL },
	{ &fnv_test_str[5], (Fnv32_t) 0xe00c22e0UL },
	{ &fnv_test_str[6], (Fnv32_t) 0xe30c2799UL },
	{ &fnv_test_str[7], (Fnv32_t) 0x6222e842UL },
	{ &fnv_test_str[8], (Fnv32_t) 0xa9f37ed7UL },
	{ &fnv_test_str[9], (Fnv32_t) 0x3f5076efUL },
	{ &fnv_test_str[10], (Fnv32_t) 0x39aaa18aUL },
	{ &fnv_test_str[11], (Fnv32_t) 0xbf9cf968UL },
	{ &fnv_test_str[12], (Fnv32_t) 0x050c5d1fUL },
	{ &fnv_test_str[13], (Fnv32_t) 0x2b24d044UL },
	{ &fnv_test_str[14], (Fnv32_t) 0x9d2c3f7fUL },
	{ &fnv_test_str[15], (Fnv32_t) 0x7729c516UL },
	{ &fnv_test_str[16], (Fnv32_t) 0xb91d6109UL },
	{ &fnv_test_str[17], (Fnv32_t) 0x931ae6a0UL },
	{ &fnv_test_str[18], (Fnv32_t) 0x052255dbUL },
	{ &fnv_test_str[19], (Fnv32_t) 0xbef39fe6UL },
	{ &fnv_test_str[20], (Fnv32_t) 0x6150ac75UL },
	{ &fnv_test_str[21], (Fnv32_t) 0x9aab3a3dUL },
	{ &fnv_test_str[22], (Fnv32_t) 0x519c4c3eUL },
	{ &fnv_test_str[23], (Fnv32_t) 0x0c1c9eb8UL },
	{ &fnv_test_str[24], (Fnv32_t) 0x5f299f4eUL },
	{ &fnv_test_str[25], (Fnv32_t) 0xef8580f3UL },
	{ &fnv_test_str[26], (Fnv32_t) 0xac297727UL },
	{ &fnv_test_str[27], (Fnv32_t) 0x4546b9c0UL },
	{ &fnv_test_str[28], (Fnv32_t) 0xbd564e7dUL },
	{ &fnv_test_str[29], (Fnv32_t) 0x6bdd5c67UL },
	{ &fnv_test_str[30], (Fnv32_t) 0xdd77ed30UL },
	{ &fnv_test_str[31], (Fnv32_t) 0xf4ca9683UL },
	{ &fnv_test_str[32], (Fnv32_t) 0x4aeb9bd0UL },
	{ &fnv_test_str[33], (Fnv32_t) 0xe0e67ad0UL },
	{ &fnv_test_str[34], (Fnv32_t) 0xc2d32fa8UL },
	{ &fnv_test_str[35], (Fnv32_t) 0x7f743fb7UL },
	{ &fnv_test_str[36], (Fnv32_t) 0x6900631fUL },
	{ &fnv_test_str[37], (Fnv32_t) 0xc59c990eUL },
	{ &fnv_test_str[38], (Fnv32_t) 0x448524fdUL },
	{ &fnv_test_str[39], (Fnv32_t) 0xd49930d5UL },
	{ &fnv_test_str[40], (Fnv32_t) 0x1c85c7caUL },
	{ &fnv_test_str[41], (Fnv32_t) 0x0229fe89UL },
	{ &fnv_test_str[42], (Fnv32_t) 0x2c469265UL },
	{ &fnv_test_str[43], (Fnv32_t) 0xce566940UL },
	{ &fnv_test_str[44], (Fnv32_t) 0x8bdd8ec7UL },
	{ &fnv_test_str[45], (Fnv32_t) 0x34787625UL },
	{ &fnv_test_str[46], (Fnv32_t) 0xd3ca6290UL },
	{ &fnv_test_str[47], (Fnv32_t) 0xddeaf039UL },
	{ &fnv_test_str[48], (Fnv32_t) 0xc0e64870UL },
	{ &fnv_test_str[49], (Fnv32_t) 0xdad35570UL },
	{ &fnv_test_str[50], (Fnv32_t) 0x5a740578UL },
	{ &fnv_test_str[51], (Fnv32_t) 0x5b004d15UL },
	{ &fnv_test_str[52], (Fnv32_t) 0x6a9c09cdUL },
	{ &fnv_test_str[53], (Fnv32_t) 0x2384f10aUL },
	{ &fnv_test_str[54], (Fnv32_t) 0xda993a47UL },
	{ &fnv_test_str[55], (Fnv32_t) 0x8227df4fUL },
	{ &fnv_test_str[56], (Fnv32_t) 0x4c298165UL },
	{ &fnv_test_str[57], (Fnv32_t) 0xfc563735UL },
	{ &fnv_test_str[58], (Fnv32_t) 0x8cb91483UL },
	{ &fnv_test_str[59], (Fnv32_t) 0x775bf5d0UL },
	{ &fnv_test_str[60], (Fnv32_t) 0xd5c428d0UL },
	{ &fnv_test_str[61], (Fnv32_t) 0x34cc0ea3UL },
	{ &fnv_test_str[62], (Fnv32_t) 0xea3b4cb7UL },
	{ &fnv_test_str[63], (Fnv32_t) 0x8e59f029UL },
	{ &fnv_test_str[64], (Fnv32_t) 0x2094de2bUL },
	{ &fnv_test_str[65], (Fnv32_t) 0xa65a0ad4UL },
	{ &fnv_test_str[66], (Fnv32_t) 0x9bbee5f4UL },
	{ &fnv_test_str[67], (Fnv32_t) 0xbe836343UL },
	{ &fnv_test_str[68], (Fnv32_t) 0x22d5344eUL },
	{ &fnv_test_str[69], (Fnv32_t) 0x19a1470cUL },
	{ &fnv_test_str[70], (Fnv32_t) 0x4a56b1ffUL },
	{ &fnv_test_str[71], (Fnv32_t) 0x70b8e86fUL },
	{ &fnv_test_str[72], (Fnv32_t) 0x0a5b4a39UL },
	{ &fnv_test_str[73], (Fnv32_t) 0xb5c3f670UL },
	{ &fnv_test_str[74], (Fnv32_t) 0x53cc3f70UL },
	{ &fnv_test_str[75], (Fnv32_t) 0xc03b0a99UL },
	{ &fnv_test_str[76], (Fnv32_t) 0x7259c415UL },
	{ &fnv_test_str[77], (Fnv32_t) 0x4095108bUL },
	{ &fnv_test_str[78], (Fnv32_t) 0x7559bdb1UL },
	{ &fnv_test_str[79], (Fnv32_t) 0xb3bf0bbcUL },
	{ &fnv_test_str[80], (Fnv32_t) 0x2183ff1cUL },
	{ &fnv_test_str[81], (Fnv32_t) 0x2bd54279UL },
	{ &fnv_test_str[82], (Fnv32_t) 0x23a156caUL },
	{ &fnv_test_str[83], (Fnv32_t) 0x64e2d7e4UL },
	{ &fnv_test_str[84], (Fnv32_t) 0x683af69aUL },
	{ &fnv_test_str[85], (Fnv32_t) 0xaed2346eUL },
	{ &fnv_test_str[86], (Fnv32_t) 0x4f9f2cabUL },
	{ &fnv_test_str[87], (Fnv32_t) 0x02935131UL },
	{ &fnv_test_str[88], (Fnv32_t) 0xc48fb86dUL },
	{ &fnv_test_str[89], (Fnv32_t) 0x2269f369UL },
	{ &fnv_test_str[90], (Fnv32_t) 0xc18fb3b4UL },
	{ &fnv_test_str[91], (Fnv32_t) 0x50ef1236UL },
	{ &fnv_test_str[92], (Fnv32_t) 0xc28fb547UL },
	{ &fnv_test_str[93], (Fnv32_t) 0x96c3bf47UL },
	{ &fnv_test_str[94], (Fnv32_t) 0xbf8fb08eUL },
	{ &fnv_test_str[95], (Fnv32_t) 0xf3e4d49cUL },
	{ &fnv_test_str[96], (Fnv32_t) 0x32179058UL },
	{ &fnv_test_str[97], (Fnv32_t) 0x280bfee6UL },
	{ &fnv_test_str[98], (Fnv32_t) 0x30178d32UL },
	{ &fnv_test_str[99], (Fnv32_t) 0x21addaf8UL },
	{ &fnv_test_str[100], (Fnv32_t) 0x4217a988UL },
	{ &fnv_test_str[101], (Fnv32_t) 0x772633d6UL },
	{ &fnv_test_str[102], (Fnv32_t) 0x08a3d11eUL },
	{ &fnv_test_str[103], (Fnv32_t) 0xb7e2323aUL },
	{ &fnv_test_str[104], (Fnv32_t) 0x07a3cf8bUL },
	{ &fnv_test_str[105], (Fnv32_t) 0x91dfb7d1UL },
	{ &fnv_test_str[106], (Fnv32_t) 0x06a3cdf8UL },
	{ &fnv_test_str[107], (Fnv32_t) 0x6bdd3d68UL },
	{ &fnv_test_str[108], (Fnv32_t) 0x1d5636a7UL },
	{ &fnv_test_str[109], (Fnv32_t) 0xd5b808e5UL },
	{ &fnv_test_str[110], (Fnv32_t) 0x1353e852UL },
	{ &fnv_test_str[111], (Fnv32_t) 0xbf16b916UL },
	{ &fnv_test_str[112], (Fnv32_t) 0xa55b89edUL },
	{ &fnv_test_str[113], (Fnv32_t) 0x3c1a2017UL },
	{ &fnv_test_str[114], (Fnv32_t) 0x0588b13cUL },
	{ &fnv_test_str[115], (Fnv32_t) 0xf22f0174UL },
	{ &fnv_test_str[116], (Fnv32_t) 0xe83641e1UL },
	{ &fnv_test_str[117], (Fnv32_t) 0x6e69b533UL },
	{ &fnv_test_str[118], (Fnv32_t) 0xf1760448UL },
	{ &fnv_test_str[119], (Fnv32_t) 0x64c8bd58UL },
	{ &fnv_test_str[120], (Fnv32_t) 0x97b4ea23UL },
	{ &fnv_test_str[121], (Fnv32_t) 0x9a4e92e6UL },
	{ &fnv_test_str[122], (Fnv32_t) 0xcfb14012UL },
	{ &fnv_test_str[123], (Fnv32_t) 0xf01b2511UL },
	{ &fnv_test_str[124], (Fnv32_t) 0x0bbb59c3UL },
	{ &fnv_test_str[125], (Fnv32_t) 0xce524afaUL },
	{ &fnv_test_str[126], (Fnv32_t) 0xdd16ef45UL },
	{ &fnv_test_str[127], (Fnv32_t) 0x60648bb3UL },
	{ &fnv_test_str[128], (Fnv32_t) 0x7fa4bcfcUL },
	{ &fnv_test_str[129], (Fnv32_t) 0x5053ae17UL },
	{ &fnv_test_str[130], (Fnv32_t) 0xc9302890UL },
	{ &fnv_test_str[131], (Fnv32_t) 0x956ded32UL },
	{ &fnv_test_str[132], (Fnv32_t) 0x9136db84UL },
	{ &fnv_test_str[133], (Fnv32_t) 0xdf9d3323UL },
	{ &fnv_test_str[134], (Fnv32_t) 0x32bb6cd0UL },
	{ &fnv_test_str[135], (Fnv32_t) 0xc8f8385bUL },
	{ &fnv_test_str[136], (Fnv32_t) 0xeb08bfbaUL },
	{ &fnv_test_str[137], (Fnv32_t) 0x62cc8e3dUL },
	{ &fnv_test_str[138], (Fnv32_t) 0xc3e20f5cUL },
	{ &fnv_test_str[139], (Fnv32_t) 0x39e97f17UL },
	{ &fnv_test_str[140], (Fnv32_t) 0x7837b203UL },
	{ &fnv_test_str[141], (Fnv32_t) 0x319e877bUL },
	{ &fnv_test_str[142], (Fnv32_t) 0xd3e63f89UL },
	{ &fnv_test_str[143], (Fnv32_t) 0x29b50b38UL },
	{ &fnv_test_str[144], (Fnv32_t) 0x5ed678b8UL },
	{ &fnv_test_str[145], (Fnv32_t) 0xb0d5b793UL },
	{ &fnv_test_str[146], (Fnv32_t) 0x52450be5UL },
	{ &fnv_test_str[147], (Fnv32_t) 0xfa72d767UL },
	{ &fnv_test_str[148], (Fnv32_t) 0x95066709UL },
	{ &fnv_test_str[149], (Fnv32_t) 0x7f52e123UL },
	{ &fnv_test_str[150], (Fnv32_t) 0x76966481UL },
	{ &fnv_test_str[151], (Fnv32_t) 0x063258b0UL },
	{ &fnv_test_str[152], (Fnv32_t) 0x2ded6e8aUL },
	{ &fnv_test_str[153], (Fnv32_t) 0xb07d7c52UL },
	{ &fnv_test_str[154], (Fnv32_t) 0xd0c71b71UL },
	{ &fnv_test_str[155], (Fnv32_t) 0xf684f1bdUL },
	{ &fnv_test_str[156], (Fnv32_t) 0x868ecfa8UL },
	{ &fnv_test_str[157], (Fnv32_t) 0xf794f684UL },
	{ &fnv_test_str[158], (Fnv32_t) 0xd19701c3UL },
	{ &fnv_test_str[159], (Fnv32_t) 0x346e171eUL },
	{ &fnv_test_str[160], (Fnv32_t) 0x91f8f676UL },
	{ &fnv_test_str[161], (Fnv32_t) 0x0bf58848UL },
	{ &fnv_test_str[162], (Fnv32_t) 0x6317b6d1UL },
	{ &fnv_test_str[163], (Fnv32_t) 0xafad4c54UL },
	{ &fnv_test_str[164], (Fnv32_t) 0x0f25681eUL },
	{ &fnv_test_str[165], (Fnv32_t) 0x91b18d49UL },
	{ &fnv_test_str[166], (Fnv32_t) 0x7d61c12eUL },
	{ &fnv_test_str[167], (Fnv32_t) 0x5147d25cUL },
	{ &fnv_test_str[168], (Fnv32_t) 0x9a8b6805UL },
	{ &fnv_test_str[169], (Fnv32_t) 0x4cd2a447UL },
	{ &fnv_test_str[170], (Fnv32_t) 0x1e549b14UL },
	{ &fnv_test_str[171], (Fnv32_t) 0x2fe1b574UL },
	{ &fnv_test_str[172], (Fnv32_t) 0xcf0cd31eUL },
	{ &fnv_test_str[173], (Fnv32_t) 0x6c471669UL },
	{ &fnv_test_str[174], (Fnv32_t) 0x0e5eef1eUL },
	{ &fnv_test_str[175], (Fnv32_t) 0x2bed3602UL },
	{ &fnv_test_str[176], (Fnv32_t) 0xb26249e0UL },
	{ &fnv_test_str[177], (Fnv32_t) 0x2c9b86a4UL },
	{ &fnv_test_str[178], (Fnv32_t) 0xe415e2bbUL },
	{ &fnv_test_str[179], (Fnv32_t) 0x18a98d1dUL },
	{ &fnv_test_str[180], (Fnv32_t) 0xb7df8b7bUL },
	{ &fnv_test_str[181], (Fnv32_t) 0x241e9075UL },
	{ &fnv_test_str[182], (Fnv32_t) 0x063f70ddUL },
	{ &fnv_test_str[183], (Fnv32_t) 0x0295aed9UL },
	{ &fnv_test_str[184], (Fnv32_t) 0x56a7f781UL },
	{ &fnv_test_str[185], (Fnv32_t) 0x253bc645UL },
	{ &fnv_test_str[186], (Fnv32_t) 0x46610921UL },
	{ &fnv_test_str[187], (Fnv32_t) 0x7c1577f9UL },
	{ &fnv_test_str[188], (Fnv32_t) 0x512b2851UL },
	{ &fnv_test_str[189], (Fnv32_t) 0x76823999UL },
	{ &fnv_test_str[190], (Fnv32_t) 0xc0586935UL },
	{ &fnv_test_str[191], (Fnv32_t) 0xf3415c85UL },
	{ &fnv_test_str[192], (Fnv32_t) 0x0ae4ff65UL },
	{ &fnv_test_str[193], (Fnv32_t) 0x58b79725UL },
	{ &fnv_test_str[194], (Fnv32_t) 0xdea43aa5UL },
	{ &fnv_test_str[195], (Fnv32_t) 0x2bb3be35UL },
	{ &fnv_test_str[196], (Fnv32_t) 0xea777a45UL },
	{ &fnv_test_str[197], (Fnv32_t) 0x8f21c305UL },
	{ &fnv_test_str[198], (Fnv32_t) 0x5c9d0865UL },
	{ &fnv_test_str[199], (Fnv32_t) 0xfa823dd5UL },
	{ &fnv_test_str[200], (Fnv32_t) 0x21a27271UL },
	{ &fnv_test_str[201], (Fnv32_t) 0x83c5c6d5UL },
	{ &fnv_test_str[202], (Fnv32_t) 0x813b0881UL },
	{ NULL, 0 }
};


bool testFnv1a()
{
	fnv1a_32_test_vector *vec = fnv1a_32_vector;
	int ok = 0, notok = 0, hashnum = 0;
	printf("Running hash tests...\n");
	while(vec->test)
	{
		//only test values where the trailing NULL was not included
		if(strlen((const char*)vec->test->buf) == vec->test->len)
		{
			uint32_t compHash = hashFnv((const char*)vec->test->buf);
			if(compHash != vec->fnv1a_32)
			{
				printf("HASH ERROR: Test #%i, Calculated: %x, Real: %x\n", hashnum, compHash, vec->fnv1a_32);
				notok++;
			}
			else
				ok++;
		}
		++vec;
		++hashnum;
	}

	if(notok == 0)
		printf("HASH: Completed %i hash tests successfully\n", ok);
	else
		printf("HASH: %i out of %i tests failed\n", notok, ok+notok);

	return notok == 0;
}


class CHashTest : public CUnitTest
{
public:
	CHashTest() : 
		CUnitTest("HashTest")
	{}

	virtual bool RunTests( void );
};


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CHashTest::RunTests( void )
{
	return testFnv1a();
}

CHashTest hashTest;

#endif //#ifndef NO_UNITTESTS