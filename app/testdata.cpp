

/*
 *  testdata.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "testdata.h"

void save_iHome_test_node_to_MySQL()
{
    MySQLDBAccess * mysqldb = new MySQLDBAccess(1);

    mysqldb->delete_all_node(NODE_TYPE_ANCHOR);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An022", "3131000000000022",  2105,   -81, 216, "32.12.0", 200, 1, 12);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_ANCHOR, "An018", "3131000000000018",   320,  -274, 230, "32.12.0", 201, 1, 12);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_ANCHOR, "An039", "3131000000000039", -2781,   -21, 216, "32.12.0", 202, 1, 12);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_ANCHOR, "An008", "3131000000000008",  2945,  -121, 216, "32.12.0", 203, 1, 12);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_ANCHOR, "An034", "3131000000000034",   293,   -44, 216, "32.12.0", 204, 1, 12);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_ANCHOR, "An036", "3131000000000036", -1093,   -63, 216, "32.12.0", 205, 1, 12);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_ANCHOR, "An035", "3131000000000035", -1740,   -63, 216, "32.12.0", 206, 1, 12);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_ANCHOR, "An009", "3131000000000009",  1069,   -81, 216, "32.12.0", 207, 1, 12);

    mysqldb->delete_all_node(NODE_TYPE_COORD);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord022", "3130000000000022",  2105,   -81, 216, "32.12.0", 500, 1, 12);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_COORD, "Coord018", "3130000000000018",   320,  -274, 230, "32.12.0", 501, 1, 12);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_COORD, "Coord039", "3130000000000039", -2781,   -21, 216, "32.12.0", 502, 1, 12);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_COORD, "Coord008", "3130000000000008",  2985,   -81, 216, "32.12.0", 503, 1, 12);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_COORD, "Coord034", "3130000000000034",   293,   -44, 216, "32.12.0", 504, 1, 12);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_COORD, "Coord036", "3130000000000036", -1093,   -63, 216, "32.12.0", 505, 1, 12);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_COORD, "Coord035", "3130000000000035", -1740,   -63, 216, "32.12.0", 506, 1, 12);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_COORD, "Coord009", "3130000000000009",  1069,   -81, 216, "32.12.0", 507, 1, 12);

    mysqldb->delete_all_node(NODE_TYPE_TAG);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_TAG, "Tag900", "0410000000000900", 0, 0, 0, "4.12.2", 900, 1, 12);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_TAG, "Tag901", "0410000000000901", 0, 0, 0, "4.12.2", 901, 1, 12);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_TAG, "Tag902", "0410000000000902", 0, 0, 0, "4.12.2", 902, 1, 12);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_TAG, "Tag903", "0410000000000903", 0, 0, 0, "4.12.2", 903, 1, 12);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_TAG, "Tag904", "0410000000000904", 0, 0, 0, "4.12.2", 904, 1, 12);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_TAG, "Tag905", "0410000000000905", 0, 0, 0, "4.12.2", 905, 1, 12);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_TAG, "Tag906", "0410000000000906", 0, 0, 0, "4.12.2", 906, 1, 12);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_TAG, "Tag907", "0410000000000907", 0, 0, 0, "4.12.2", 907, 1, 12);
    save_node_to_MySQL(mysqldb,  8, NODE_TYPE_TAG, "Tag908", "0410000000000908", 0, 0, 0, "4.12.2", 908, 1, 12);
    save_node_to_MySQL(mysqldb,  9, NODE_TYPE_TAG, "Tag909", "0410000000000909", 0, 0, 0, "4.12.2", 909, 1, 12);
    save_node_to_MySQL(mysqldb, 10, NODE_TYPE_TAG, "Tag910", "0410000000000910", 0, 0, 0, "4.12.2", 910, 1, 12);

    delete mysqldb;
}


void save_iHome_base_node_to_MySQL()
{
    MySQLDBAccess * mysqldb = new MySQLDBAccess(1);


    // 12F
    mysqldb->delete_all_node(NODE_TYPE_ANCHOR);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An022", "3131000000000022",  2105,   -81, 216, "32.12.0", 200, 1, 12);
    //save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An007", "3131000000000007",  2105,   -81, 216, "32.12.0", 200, 1, 12);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_ANCHOR, "An018", "3131000000000018",   320,  -274, 230, "32.12.0", 201, 1, 12);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_ANCHOR, "An039", "3131000000000039", -2781,   -21, 216, "32.12.0", 202, 1, 12);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_ANCHOR, "An033", "3131000000000033",   460,  -230, 216, "32.12.0", 203, 1, 12);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_ANCHOR, "An034", "3131000000000034",   293,   -44, 216, "32.12.0", 204, 1, 12);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_ANCHOR, "An036", "3131000000000036", -1093,   -63, 216, "32.12.0", 205, 1, 12);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_ANCHOR, "An035", "3131000000000035", -1740,   -63, 216, "32.12.0", 206, 1, 12);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_ANCHOR, "An037", "3131000000000037", -2810, -1477, 216, "32.12.0", 207, 1, 12);
    //save_node_to_MySQL(mysqldb,  8, NODE_TYPE_ANCHOR, "An032", "3131000000000032",  1069,   -81, 216, "32.12.0", 208, 1, 12);
    save_node_to_MySQL(mysqldb,  8, NODE_TYPE_ANCHOR, "An009", "3131000000000009",  1069,   -81, 216, "32.12.0", 208, 1, 12);
    save_node_to_MySQL(mysqldb,  9, NODE_TYPE_ANCHOR, "An040", "3131000000000040",  4564, -4673, 216, "32.12.0", 209, 1, 12);

    save_node_to_MySQL(mysqldb,  10, NODE_TYPE_ANCHOR, "An038", "3131000000000038",  3949, -4591, 216, "32.12.0", 210, 1, 12);
    save_node_to_MySQL(mysqldb,  11, NODE_TYPE_ANCHOR, "An030", "3131000000000030",  1761, -4591, 216, "32.12.0", 211, 1, 12);
    save_node_to_MySQL(mysqldb,  12, NODE_TYPE_ANCHOR, "An028", "3131000000000028",  1536, -4804, 216, "32.12.0", 212, 1, 12);
    save_node_to_MySQL(mysqldb,  13, NODE_TYPE_ANCHOR, "An031", "3131000000000031",  1580, -4499, 216, "32.12.0", 213, 1, 12);
    save_node_to_MySQL(mysqldb,  14, NODE_TYPE_ANCHOR, "An027", "3131000000000027",   182, -4676, 216, "32.12.0", 214, 1, 12);
    save_node_to_MySQL(mysqldb,  15, NODE_TYPE_ANCHOR, "An023", "3131000000000023", -1900, -4671, 216, "32.12.0", 215, 1, 12);
    save_node_to_MySQL(mysqldb,  16, NODE_TYPE_ANCHOR, "An024", "3131000000000024", -1712, -4591, 216, "32.12.0", 216, 1, 12);
    save_node_to_MySQL(mysqldb,  17, NODE_TYPE_ANCHOR, "An021", "3131000000000021", -2853, -4619, 216, "32.12.0", 217, 1, 12);
    save_node_to_MySQL(mysqldb,  18, NODE_TYPE_ANCHOR, "An019", "3131000000000019", -2835, -4195, 216, "32.12.0", 218, 1, 12);
    save_node_to_MySQL(mysqldb,  19, NODE_TYPE_ANCHOR, "An020", "3131000000000020", -2941, -2725, 216, "32.12.0", 219, 1, 12);

    save_node_to_MySQL(mysqldb,  20, NODE_TYPE_ANCHOR, "An029", "3131000000000029", -2717, -1046, 216, "32.12.0", 220, 1, 12);
    save_node_to_MySQL(mysqldb,  21, NODE_TYPE_ANCHOR, "An064", "3131000000000064", -2084, -4405, 216, "32.12.0", 221, 1, 12);
    save_node_to_MySQL(mysqldb,  22, NODE_TYPE_ANCHOR, "An005", "3131000000000005", -1183, -4715, 216, "32.12.0", 222, 1, 12);
    save_node_to_MySQL(mysqldb,  23, NODE_TYPE_ANCHOR, "An006", "3131000000000006",  2914, -4716, 216, "32.12.0", 223, 1, 12);
    save_node_to_MySQL(mysqldb,  24, NODE_TYPE_ANCHOR, "An059", "3131000000000059", -2812, -2301, 216, "32.12.0", 224, 1, 12);
    save_node_to_MySQL(mysqldb,  25, NODE_TYPE_ANCHOR, "An008", "3131000000000008",  2945,  -121, 216, "32.12.0", 225, 1, 12);
    save_node_to_MySQL(mysqldb,  26, NODE_TYPE_ANCHOR, "An003", "3131000000000003",  3050,  -173, 216, "32.12.0", 226, 1, 12);

    // 13F
    save_node_to_MySQL(mysqldb,  27, NODE_TYPE_ANCHOR, "An010", "3131000000000010",  1532, -4727, 219, "32.12.0", 227, 1, 13);
    save_node_to_MySQL(mysqldb,  28, NODE_TYPE_ANCHOR, "An041", "3131000000000041",  2247,  -127, 219, "32.12.0", 228, 1, 13);
    save_node_to_MySQL(mysqldb,  29, NODE_TYPE_ANCHOR, "An042", "3131000000000042",  1683,   -76, 219, "32.12.0", 229, 1, 13);
    save_node_to_MySQL(mysqldb,  30, NODE_TYPE_ANCHOR, "An053", "3131000000000053",  -284, -4726, 219, "32.12.0", 230, 1, 13);
    save_node_to_MySQL(mysqldb,  31, NODE_TYPE_ANCHOR, "An043", "3131000000000043",   471,  -222, 258, "32.12.0", 231, 1, 13);
    save_node_to_MySQL(mysqldb,  32, NODE_TYPE_ANCHOR, "An061", "3131000000000061", -1778, -4595, 238, "32.12.0", 232, 1, 13);
    save_node_to_MySQL(mysqldb,  33, NODE_TYPE_ANCHOR, "An046", "3131000000000046", -1058,  -126, 219, "32.12.0", 233, 1, 13);
    save_node_to_MySQL(mysqldb,  34, NODE_TYPE_ANCHOR, "An048", "3131000000000048", -2806, -1472, 218, "32.12.0", 234, 1, 13);
    save_node_to_MySQL(mysqldb,  35, NODE_TYPE_ANCHOR, "An044", "3131000000000044",   275,  -377, 230, "32.12.0", 235, 1, 13);
    save_node_to_MySQL(mysqldb,  36, NODE_TYPE_ANCHOR, "An045", "3131000000000045",   290,   -76, 216, "32.12.0", 236, 1, 13);

    save_node_to_MySQL(mysqldb,  37, NODE_TYPE_ANCHOR, "An047", "3131000000000047", -2855,   -37, 215, "32.12.0", 237, 1, 13);
    save_node_to_MySQL(mysqldb,  38, NODE_TYPE_ANCHOR, "An049", "3131000000000049", -2806, -1472, 218, "32.12.0", 238, 1, 13);
    save_node_to_MySQL(mysqldb,  39, NODE_TYPE_ANCHOR, "An055", "3131000000000055",  1508, -4468, 216, "32.12.0", 239, 1, 13);
    save_node_to_MySQL(mysqldb,  40, NODE_TYPE_ANCHOR, "An050", "3131000000000050", -2856, -2625, 218, "32.12.0", 240, 1, 13);
    save_node_to_MySQL(mysqldb,  41, NODE_TYPE_ANCHOR, "An051", "3131000000000051", -2860, -4650, 217, "32.12.0", 241, 1, 13);
    save_node_to_MySQL(mysqldb,  42, NODE_TYPE_ANCHOR, "An052", "3131000000000052", -2069, -4726, 219, "32.12.0", 242, 1, 13);
    save_node_to_MySQL(mysqldb,  43, NODE_TYPE_ANCHOR, "An060", "3131000000000060", -2934, -2659, 240, "32.12.0", 243, 1, 13);
    save_node_to_MySQL(mysqldb,  44, NODE_TYPE_ANCHOR, "An058", "3131000000000058",  4500, -4717, 216, "32.12.0", 244, 1, 13);
    save_node_to_MySQL(mysqldb,  45, NODE_TYPE_ANCHOR, "An057", "3131000000000057",  4022, -4600, 245, "32.12.0", 245, 1, 13);

    save_node_to_MySQL(mysqldb,  46, NODE_TYPE_ANCHOR, "An026", "3131000000000026",  2891, -4681, 219, "32.12.0", 246, 1, 13);
    save_node_to_MySQL(mysqldb,  47, NODE_TYPE_ANCHOR, "An065", "3131000000000065",  3059,  -102, 236, "32.12.0", 247, 1, 13);
    save_node_to_MySQL(mysqldb,  48, NODE_TYPE_ANCHOR, "An066", "3131000000000066",  2951,   -73, 219, "32.12.0", 248, 1, 13);
    save_node_to_MySQL(mysqldb,  49, NODE_TYPE_ANCHOR, "An067", "3131000000000067", -1752,   -77, 219, "32.12.0", 249, 1, 13);
    save_node_to_MySQL(mysqldb,  50, NODE_TYPE_ANCHOR, "An068", "3131000000000068", -2792, -4109, 219, "32.12.0", 250, 1, 13);
    save_node_to_MySQL(mysqldb,  51, NODE_TYPE_ANCHOR, "An069", "3131000000000069", -2106, -4444, 228, "32.12.0", 251, 1, 13);
    save_node_to_MySQL(mysqldb,  52, NODE_TYPE_ANCHOR, "An070", "3131000000000070", -1090, -4675, 219, "32.12.0", 252, 1, 13);
    save_node_to_MySQL(mysqldb,  53, NODE_TYPE_ANCHOR, "An025", "3131000000000025",  1753, -4595, 240, "32.12.0", 253, 1, 13);

    // 12F LoRA coordinator
    //save_node_to_MySQL(mysqldb,  54, NODE_TYPE_ANCHOR, "Locator021", "3130000000003121",  -130, -800, 100, "32.40.1", 254, 1, 12);
    save_node_to_MySQL(mysqldb,  54, NODE_TYPE_ANCHOR, "Locator021", "3130000000003121",  -1732, -715, 100, "32.40.1", 254, 1, 12);

    //save_node_to_MySQL(mysqldb,  55, NODE_TYPE_ANCHOR, "An219", "0320000000000019",  3039,-63,216,  "3.12.0", 255, 1, 12);


    // 12F
    mysqldb->delete_all_node(NODE_TYPE_COORD);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord022", "3130000000000022",  2105,   -81, 216, "32.12.0", 500, 1, 12);
    //save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord007", "3130000000000007",  2105,   -81, 216, "32.12.0", 500, 1, 12);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_COORD, "Coord018", "3130000000000018",   320,  -274, 230, "32.12.0", 501, 1, 12);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_COORD, "Coord039", "3130000000000039", -2781,   -21, 216, "32.12.0", 502, 1, 12);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_COORD, "Coord033", "3130000000000033",   460,  -530, 216, "32.12.0", 503, 1, 12);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_COORD, "Coord034", "3130000000000034",   293,   -44, 216, "32.12.0", 504, 1, 12);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_COORD, "Coord036", "3130000000000036", -1093,   -63, 216, "32.12.0", 505, 1, 12);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_COORD, "Coord035", "3130000000000035", -1740,   -63, 216, "32.12.0", 506, 1, 12);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_COORD, "Coord037", "3130000000000037", -2810, -1477, 216, "32.12.0", 507, 1, 12);
    //save_node_to_MySQL(mysqldb,  8, NODE_TYPE_COORD, "Coord032", "3130000000000032",  1069,   -81, 216, "32.12.0", 508, 1, 12);
    save_node_to_MySQL(mysqldb,  8, NODE_TYPE_COORD, "Coord009", "3130000000000009",  1069,   -81, 216, "32.12.0", 508, 1, 12);
    save_node_to_MySQL(mysqldb,  9, NODE_TYPE_COORD, "Coord040", "3130000000000040",  4564, -4673, 216, "32.12.0", 509, 1, 12);

    save_node_to_MySQL(mysqldb,  10, NODE_TYPE_COORD, "Coord038", "3130000000000038",  3949, -4591, 216, "32.12.0", 510, 1, 12);
    save_node_to_MySQL(mysqldb,  11, NODE_TYPE_COORD, "Coord030", "3130000000000030",  1761, -4591, 216, "32.12.0", 511, 1, 12);
    save_node_to_MySQL(mysqldb,  12, NODE_TYPE_COORD, "Coord028", "3130000000000028",  1536, -4804, 216, "32.12.0", 512, 1, 12);
    save_node_to_MySQL(mysqldb,  13, NODE_TYPE_COORD, "Coord031", "3130000000000031",  1580, -4499, 216, "32.12.0", 513, 1, 12);
    save_node_to_MySQL(mysqldb,  14, NODE_TYPE_COORD, "Coord027", "3130000000000027",   182, -4676, 216, "32.12.0", 514, 1, 12);
    save_node_to_MySQL(mysqldb,  15, NODE_TYPE_COORD, "Coord023", "3130000000000023", -1900, -4671, 216, "32.12.0", 515, 1, 12);
    save_node_to_MySQL(mysqldb,  16, NODE_TYPE_COORD, "Coord024", "3130000000000024", -1712, -4591, 216, "32.12.0", 516, 1, 12);
    save_node_to_MySQL(mysqldb,  17, NODE_TYPE_COORD, "Coord021", "3130000000000021", -2853, -4619, 216, "32.12.0", 517, 1, 12);
    save_node_to_MySQL(mysqldb,  18, NODE_TYPE_COORD, "Coord019", "3130000000000019", -2835, -4195, 216, "32.12.0", 518, 1, 12);
    save_node_to_MySQL(mysqldb,  19, NODE_TYPE_COORD, "Coord020", "3130000000000020", -2941, -2725, 216, "32.12.0", 519, 1, 12);

    save_node_to_MySQL(mysqldb,  20, NODE_TYPE_COORD, "Coord029", "3130000000000029", -2717, -1046, 216, "32.12.0", 520, 1, 12);
    save_node_to_MySQL(mysqldb,  21, NODE_TYPE_COORD, "Coord064", "3130000000000064", -2084, -4405, 216, "32.12.0", 521, 1, 12);
    save_node_to_MySQL(mysqldb,  22, NODE_TYPE_COORD, "Coord005", "3130000000000005", -1183, -4715, 216, "32.12.0", 522, 1, 12);
    save_node_to_MySQL(mysqldb,  23, NODE_TYPE_COORD, "Coord006", "3130000000000006",  2914, -4716, 216, "32.12.0", 523, 1, 12);
    save_node_to_MySQL(mysqldb,  24, NODE_TYPE_COORD, "Coord059", "3130000000000059", -2812, -2301, 216, "32.12.0", 524, 1, 12);
    save_node_to_MySQL(mysqldb,  25, NODE_TYPE_COORD, "Coord008", "3130000000000008",  2985,   -81, 216, "32.12.0", 525, 1, 12);
    save_node_to_MySQL(mysqldb,  26, NODE_TYPE_COORD, "Coord003", "3130000000000003",  3050,  -173, 216, "32.12.0", 526, 1, 12);

    // 13F
    save_node_to_MySQL(mysqldb,  27, NODE_TYPE_COORD, "Coord010", "3130000000000010",  1532, -4727, 219, "32.12.0", 527, 1, 13);
    save_node_to_MySQL(mysqldb,  28, NODE_TYPE_COORD, "Coord041", "3130000000000041",  2247,  -127, 219, "32.12.0", 528, 1, 13);
    save_node_to_MySQL(mysqldb,  29, NODE_TYPE_COORD, "Coord042", "3130000000000042",  1683,   -76, 219, "32.12.0", 529, 1, 13);
    save_node_to_MySQL(mysqldb,  30, NODE_TYPE_COORD, "Coord053", "3130000000000053",  -284, -4726, 219, "32.12.0", 530, 1, 13);
    save_node_to_MySQL(mysqldb,  31, NODE_TYPE_COORD, "Coord043", "3130000000000043",   471,  -222, 258, "32.12.0", 531, 1, 13);
    save_node_to_MySQL(mysqldb,  32, NODE_TYPE_COORD, "Coord061", "3130000000000061", -1778, -4595, 238, "32.12.0", 532, 1, 13);
    save_node_to_MySQL(mysqldb,  33, NODE_TYPE_COORD, "Coord046", "3130000000000046", -1058,  -126, 219, "32.12.0", 533, 1, 13);
    save_node_to_MySQL(mysqldb,  34, NODE_TYPE_COORD, "Coord048", "3130000000000048", -2806, -1472, 218, "32.12.0", 534, 1, 13);
    save_node_to_MySQL(mysqldb,  35, NODE_TYPE_COORD, "Coord044", "3130000000000044",   275,  -377, 230, "32.12.0", 535, 1, 13);
    save_node_to_MySQL(mysqldb,  36, NODE_TYPE_COORD, "Coord045", "3130000000000045",   290,   -76, 216, "32.12.0", 536, 1, 13);

    save_node_to_MySQL(mysqldb,  37, NODE_TYPE_COORD, "Coord047", "3130000000000047", -2855,   -37, 215, "32.12.0", 537, 1, 13);
    save_node_to_MySQL(mysqldb,  38, NODE_TYPE_COORD, "Coord049", "3130000000000049", -2806, -1472, 218, "32.12.0", 538, 1, 13);
    save_node_to_MySQL(mysqldb,  39, NODE_TYPE_COORD, "Coord055", "3130000000000055",  1508, -4468, 216, "32.12.0", 539, 1, 13);
    save_node_to_MySQL(mysqldb,  40, NODE_TYPE_COORD, "Coord050", "3130000000000050", -2856, -2625, 218, "32.12.0", 540, 1, 13);
    save_node_to_MySQL(mysqldb,  41, NODE_TYPE_COORD, "Coord051", "3130000000000051", -2860, -4650, 217, "32.12.0", 541, 1, 13);
    save_node_to_MySQL(mysqldb,  42, NODE_TYPE_COORD, "Coord052", "3130000000000052", -2069, -4726, 219, "32.12.0", 542, 1, 13);
    save_node_to_MySQL(mysqldb,  43, NODE_TYPE_COORD, "Coord060", "3130000000000060", -2934, -2659, 240, "32.12.0", 543, 1, 13);
    save_node_to_MySQL(mysqldb,  44, NODE_TYPE_COORD, "Coord058", "3130000000000058",  4500, -4717, 216, "32.12.0", 544, 1, 13);
    save_node_to_MySQL(mysqldb,  45, NODE_TYPE_COORD, "Coord057", "3130000000000057",  4022, -4600, 245, "32.12.0", 545, 1, 13);

    save_node_to_MySQL(mysqldb,  46, NODE_TYPE_COORD, "Coord026", "3130000000000026",  2891, -4681, 219, "32.12.0", 546, 1, 13);
    save_node_to_MySQL(mysqldb,  47, NODE_TYPE_COORD, "Coord065", "3130000000000065",  3059,  -102, 236, "32.12.0", 547, 1, 13);
    save_node_to_MySQL(mysqldb,  48, NODE_TYPE_COORD, "Coord066", "3130000000000066",  2951,   -73, 219, "32.12.0", 548, 1, 13);
    save_node_to_MySQL(mysqldb,  49, NODE_TYPE_COORD, "Coord067", "3130000000000067", -1752,   -77, 219, "32.12.0", 549, 1, 13);
    save_node_to_MySQL(mysqldb,  50, NODE_TYPE_COORD, "Coord068", "3130000000000068", -2792, -4109, 219, "32.12.0", 550, 1, 13);
    save_node_to_MySQL(mysqldb,  51, NODE_TYPE_COORD, "Coord069", "3130000000000069", -2106, -4444, 228, "32.12.0", 551, 1, 13);
    save_node_to_MySQL(mysqldb,  52, NODE_TYPE_COORD, "Coord070", "3130000000000070", -1090, -4675, 219, "32.12.0", 552, 1, 13);
    save_node_to_MySQL(mysqldb,  53, NODE_TYPE_COORD, "Coord025", "3130000000000025",  1753, -4595, 240, "32.12.0", 553, 1, 13);

    mysqldb->delete_all_node(NODE_TYPE_TAG);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_TAG, "Tag011", "0410000000000011",   200,     0,    0, "4.12.2", 200, 1, 12);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_TAG, "TagSOS", "3130000000003121", -1732, -715, 100, "32.12.0", 201, 1, 12);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_TAG, "Tag007", "0410000000000007",  400,     0,    0, "4.12.2", 202, 1, 12);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_TAG, "Tag010", "0410000000000010",  600,     0,    0, "4.12.2", 203, 1, 12);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_TAG, "Tag016", "0410000000000016",  1536, -5000,  120, "4.12.2", 204, 1, 12);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_TAG, "Tag018", "0410000000000018",  800,     0,    0, "4.12.2", 205, 1, 12);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_TAG, "Tag017", "0410000000000017",  900,     0,    0, "4.12.2", 206, 1, 12);


    delete mysqldb;
}

void save_TAF_base_node_to_MySQL()
{

    MySQLDBAccess * mysqldb = new MySQLDBAccess(1);

    //2018/12/24
    // 1F coordinator
    mysqldb->delete_all_node(NODE_TYPE_COORD);
    save_node_to_MySQL(mysqldb, 0, NODE_TYPE_COORD, "Coord014", "3130000000000014",  4580, 20, 245, "32.12.5", 0, 1, 1);

    // 1F anchor
    mysqldb->delete_all_node(NODE_TYPE_ANCHOR);
    save_node_to_MySQL(mysqldb, 0, NODE_TYPE_ANCHOR, "An014", "3131000000000014", 4580,  20, 245, "32.12.5", 0, 1, 1);
    save_node_to_MySQL(mysqldb, 1, NODE_TYPE_ANCHOR, "An054", "0320000000000054", 4563,-214, 304, "3.12.3", 0, 1, 1);
    save_node_to_MySQL(mysqldb, 2, NODE_TYPE_ANCHOR, "An056", "0320000000000056", 4543, 505, 298, "4.12.3", 0, 1, 1);
    save_node_to_MySQL(mysqldb, 3, NODE_TYPE_ANCHOR, "An075", "0320000000000075", 5127, 570, 344, "3.12.3", 0, 1, 1);
    save_node_to_MySQL(mysqldb, 4, NODE_TYPE_ANCHOR, "An055", "0320000000000055", 5127,-214, 308, "4.12.3", 0, 1, 1);

    // tag
    mysqldb->delete_all_node(NODE_TYPE_TAG);
    save_node_to_MySQL(mysqldb, 0, NODE_TYPE_TAG, "Tag012", "0410000000000012", 4960, 100, 100, "4.12.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb, 1, NODE_TYPE_TAG, "Tag999", "0410000000000999", 4960, 200, 100, "4.12.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb, 2, NODE_TYPE_TAG, "Tag020", "0610000000000020", 4960, 300, 100, "5.10.0", 0, 1, 1);

    // locator
    mysqldb->delete_all_node(NODE_TYPE_LOCATOR);
    save_node_to_MySQL(mysqldb, 0, NODE_TYPE_LOCATOR, "Locator0013", "0530000000000013", "0000000000050013", 5000, 0, 100, "33.11.5", 0, 1, 1);


    // 2F coordinator
    //save_TAF_node_to_MySQL(mysqldb, 4, 2, "An034", "0230000000000034",  615,   200, 645, "2.10.5", 154, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb, 5, 2, "An033", "0230000000000033", 3331,   100, 645, "2.10.5", 155, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb, 6, 2, "An001", "0230000000000001", 4565,    50, 645, "2.10.5", 156, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb, 7, 2, "An032", "0230000000000032", 3970,  -500, 645, "2.10.5", 157, 16, 2);

    //save_TAF_node_to_MySQL(mysqldb, 2, 2, "An031", "0230000000000031", 4350, -1203, 289, "2.10.5", 152, 16, 1);

    // 1F anchor
    //save_TAF_node_to_MySQL(mysqldb,10, 0, "An056", "0220000000000056", 3325,   100, 289, "2.10.2", 110, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb,11, 0, "An060", "0220000000000060", 2673,   115, 289, "2.10.2", 111, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb,12, 0, "An061", "0220000000000061", 2575,    74, 289, "2.10.2", 112, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb,14, 0, "An055", "0220000000000055", 1825,   120, 289, "2.10.2", 114, 16, 1);

    // 2F anchor
    //save_TAF_node_to_MySQL(mysqldb,28, 0, "An096", "0220000000000096", 2060,     0, 585, "2.10.2", 128, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,29, 0, "An059", "0220000000000059", 1825,   104, 645, "2.10.2", 129, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,30, 0, "An052", "0220000000000052",  885,   112, 645, "2.10.2", 130, 16, 2);

    // // 1F anchor
    // save_node_to_MySQL(mysqldb, 0, 0, "An305", "0320000000000005", 4500,  -524, 299, "3.10.8", 100, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 0, 0, "An082", "0220000000000082", 4500,  -524, 299, "2.10.2", 100, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 1, 0, "An037", "0220000000000037", 4475,  -430, 240, "2.10.2", 101, 16, 1);
    // save_node_to_MySQL(mysqldb, 1, 0, "An375", "0320000000000075", 4472,  -985, 289, "3.10.8", 101, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 1, 0, "An057", "0220000000000057", 4472,  -985, 289, "2.10.2", 101, 16, 1);
    // save_node_to_MySQL(mysqldb, 2, 0, "An354", "0320000000000054", 4500,   -75, 299, "3.10.8", 102, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 2, 0, "An080", "0220000000000080", 4500,   -75, 299, "2.10.2", 102, 16, 1);
    // save_node_to_MySQL(mysqldb, 3, 0, "An081", "0220000000000081", 5080,     0, 337, "2.10.2", 103, 16, 1);
    // save_node_to_MySQL(mysqldb, 4, 0, "An304", "0320000000000004", 5060,  -557, 285, "3.10.8", 104, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 4, 0, "An056", "0220000000000056", 5060,  -557, 285, "2.10.2", 104, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 4, 0, "An083", "0220000000000083", 5060,  -557, 285, "2.10.2", 104, 16, 1);
    // save_node_to_MySQL(mysqldb, 5, 0, "An096", "0220000000000096", 4530,  -580,  50, "2.10.2", 105, 16, 1);
    // save_node_to_MySQL(mysqldb, 6, 0, "An356", "0320000000000056", 4496,     0, 289, "3.10.8", 106, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 6, 0, "An066", "0220000000000066", 4496,     0, 289, "2.10.2", 106, 16, 1);
    // save_node_to_MySQL(mysqldb, 7, 0, "An380", "0320000000000080", 4500,   210, 289, "3.10.8", 107, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 7, 0, "An064", "0220000000000064", 4500,   210, 289, "2.10.2", 107, 16, 1);
    // save_node_to_MySQL(mysqldb, 8, 0, "An098", "0220000000000098", 4210,   210, 300, "2.10.2", 108, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb, 8, 0, "An065", "0220000000000065", 4216,   110, 289, "2.10.2", 108, 16, 1);
    // save_node_to_MySQL(mysqldb, 9, 0, "An100", "0220000000000100", 4210,     0, 300, "2.10.2", 109, 16, 1);
    // save_node_to_MySQL(mysqldb,10, 0, "An065", "0220000000000065", 3315,     0, 300, "2.10.2", 110, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,10, 0, "An056", "0220000000000056", 4530,  -600,  70, "2.10.2", 110, 16, 1);
    // save_node_to_MySQL(mysqldb,11, 0, "An381", "0320000000000081", 3315,   210, 300, "3.10.8", 111, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,11, 0, "An073", "0220000000000073", 3315,   210, 300, "2.10.6", 111, 16, 1);
    // save_node_to_MySQL(mysqldb,12, 0, "An060", "0220000000000060", 2660,   210, 300, "2.10.2", 112, 16, 1);
    // save_node_to_MySQL(mysqldb,13, 0, "An378", "0320000000000078", 2660,     0, 300, "3.10.8", 113, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,13, 0, "An061", "0220000000000061", 2660,     0, 300, "2.10.2", 113, 16, 1);
    // save_node_to_MySQL(mysqldb,14, 0, "An377", "0320000000000077", 4215,  -987, 300, "3.10.8", 114, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,14, 0, "An062", "0220000000000062", 4215,  -987, 300, "2.10.2", 114, 16, 1);
    // save_node_to_MySQL(mysqldb,15, 0, "An379", "0320000000000079", 1800,   210, 300, "3.10.8", 115, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,15, 0, "An055", "0220000000000055", 1800,   210, 300, "2.10.2", 115, 16, 1);
    // save_node_to_MySQL(mysqldb,16, 0, "An384", "0320000000000084", 1800,     0, 300, "3.10.8", 116, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,16, 0, "An087", "0220000000000087", 1800,     0, 300, "2.10.6", 116, 16, 1);
    // save_node_to_MySQL(mysqldb,17, 0, "An099", "0220000000000099",  900,     0, 300, "2.10.2", 117, 16, 1);
    // save_node_to_MySQL(mysqldb,18, 0, "An382", "0320000000000082",  900,   210, 300, "3.10.8", 118, 16, 1);
    // //save_TAF_node_to_MySQL(mysqldb,18, 0, "An043", "0220000000000043",  900,   210, 300, "2.10.2", 118, 16, 1);
    // save_node_to_MySQL(mysqldb,19, 0, "An036", "0220000000000036",  300,   210, 300, "2.10.2", 119, 16, 1);
    // save_node_to_MySQL(mysqldb,20, 0, "An035", "0220000000000035",  300,     0, 300, "2.10.2", 120, 16, 1);

    // 2F anchor
    //save_TAF_node_to_MySQL(mysqldb,21, 0, "An063", "0220000000000063", 4500,    84, 645, "2.10.2", 121, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,22, 0, "An067", "0220000000000067", 4565,   212, 645, "2.10.2", 122, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,23, 0, "An048", "0220000000000048", 4220,   130, 645, "2.10.2", 123, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,24, 0, "An098", "0220000000000098", 3820,    30, 662, "2.10.2", 124, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,25, 0, "An068", "0220000000000068", 3970,  -100, 645, "2.10.2", 125, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,26, 0, "An088", "0220000000000088", 3231,  -100, 645, "2.10.2", 126, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,27, 0, "An069", "0220000000000069", 3231,  -355, 645, "2.10.2", 127, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,28, 0, "An097", "0220000000000097", 2943,   203, 660, "2.10.2", 128, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,29, 0, "An005", "0220000000000005", 2400,   203, 660, "2.10.2", 129, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,30, 0, "An074", "0220000000000074", 2400,     5, 660, "2.10.6", 130, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,31, 0, "An059", "0220000000000059", 1530,   153, 652, "2.10.2", 131, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,32, 0, "An075", "0220000000000075", 1530,    53, 652, "2.10.6", 132, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,33, 0, "An052", "0220000000000052",  875,   153, 652, "2.10.2", 133, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,34, 0, "An054", "0220000000000054",  875,     0, 660, "2.10.2", 134, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,35, 0, "An053", "0220000000000053",  615,    71, 645, "2.10.2", 135, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,36, 0, "An050", "0220000000000050", 4200, -1552, 645, "2.10.2", 136, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,37, 0, "An051", "0220000000000051", 4175, -1077, 645, "2.10.2", 137, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,38, 0, "An090", "0220000000000090", 3670,  -852, 645, "2.10.2", 138, 16, 2);
    //save_TAF_node_to_MySQL(mysqldb,39, 0, "An091", "0220000000000091", 3600, -1372, 645, "2.10.2", 139, 16, 2);

    // tag
    //save_node_to_MySQL(mysqldb, 2, 1, "Tag020", "0610000000000020", 4960, 100, 100, "5.10.0", 0, 1, 1);

    //save_node_to_MySQL(mysqldb, 0, 1, "WilliamWu", "0210000000000019", 4500, -524, 100, "2.10.9", 160, 16, 1);
    //save_node_to_MySQL(mysqldb, 1, 1,     "apple", "0210000000000024", 3850, -900, 100, "2.10.9", 161, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 1, 1,       "dog", "0210000000000006", 3650, -900, 100, "2.10.9", 161, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 2, 1,       "dog", "0210000000000021", 3850, -900, 100, "2.10.7", 161, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 3, 1,     "apple", "0310000000000024", 3850, -900, 100, "2.10.9", 163, 16, 1);

    //save_TAF_node_to_MySQL(mysqldb, 1, 1,    "grapes", "0210000000000015", 3650, -900, 100, "2.10.7", 161, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 2, 1,      "kiwi", "0210000000000018", 3850, -900, 100, "2.10.7", 162, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 3, 1,     "lemon", "0210000000000017", 4050, -900, 100, "2.10.7", 163, 16, 1);

    //save_TAF_node_to_MySQL(mysqldb, 4, 1,   "chicken", "0210000000000014", 3100,  350, 100, "2.10.7", 164, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 5, 1,     "apple", "0210000000000024", 3400,  350, 100, "2.10.7", 165, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 6, 1,    "cherry", "0210000000000022", 4780, -524, 100, "2.10.6", 166, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 7, 1,      "nemo", "0210000000000023", 3700,  350, 100, "2.10.7", 167, 16, 1);
    //save_TAF_node_to_MySQL(mysqldb, 8, 1,    "rabbit", "0210000000000010", 4000,  350, 100, "2.10.7", 168, 16, 1);

    //save_TAF_node_to_MySQL(mysqldb, 1, 1,       "dog", "0210000000000006", 4540, -524, 100, "2.10.7", 161, 16, 1);

    // create base config
    // mysqldb->write_config(001, "空總聯合創新中心1F", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538550425548,  25.0402272235039, 90.00000000, 121.538550425548,  25.0402272235039, 0.00010000, 0.00010000, 16, 1);
    // mysqldb->write_config(002, "空總聯合創新中心2F", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538550425548,  25.0402272235039, 90.00000000, 121.538550425548,  25.0402272235039, 0.00010000, 0.00010000, 16, 2);
    // mysqldb->write_config(003, "空總聯合創新中心側門入口", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538550425548,  25.0402272235039, 90.00000000, 121.538550425548,  25.0402272235039, 0.00010000, 0.00010000, 16, 1);
    //
    // //mysqldb->write_config(001, "空總聯合創新中心1F", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538650425548,  25.0402192235039, 90.00000000, 121.538650425548,  25.0402192235039, 0.00010000, 0.00010000, 16, 1);
    // //mysqldb->write_config(002, "空總聯合創新中心2F", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538650425548,  25.0402192235039, 90.00000000, 121.538650425548,  25.0402192235039, 0.00010000, 0.00010000, 16, 2);
    // //mysqldb->write_config(003, "空總聯合創新中心側門入口", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538650425548,  25.0402192235039, 90.00000000, 121.538650425548,  25.0402192235039, 0.00010000, 0.00010000, 16, 1);
    //
    // //mysqldb->write_config(001, "空總聯合創新中心1F", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.53795624,  25.04014587, 0.00000000, 121.53795624,  25.04014587, 0.00010000, 0.00010000, 16, 1);
    // //mysqldb->write_config(002, "空總聯合創新中心2F", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.53795624,  25.04014587, 0.00000000, 121.53795624,  25.04014587, 0.00010000, 0.00010000, 16, 2);
    // //mysqldb->write_config(003, "空總聯合創新中心側門入口", "台北市大安區建國南路一段177號", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.53795624,  25.04014587, 0.00000000, 121.53795624,  25.04014587, 0.00010000, 0.00010000, 16, 1);
    //
    // // create project map
    // mysqldb->write_map(1, 1, "map", "104.32319726320493", "1620.1558127352334", "58.566754588815", "100000", "/upload/TAF_1F.png", "3000", "1500");
    // mysqldb->write_map(2, 2, "map", "103.32964300355536", "1617.5063347095013", "58.57261185", "100000", "/upload/TAF_2F.png", "3000", "1500");
    // mysqldb->write_map(3, 3, "map", "1975.185868183347", "1377.066203874305", "58.57261185", "100000", "/upload/TAF_Entrance.png", "1500", "3000");

    delete mysqldb;
}

void save_test_node_to_MySQL()
{
    MySQLDBAccess * mysqldb = new MySQLDBAccess(1);

    // Tag
    mysqldb->delete_all_node(NODE_TYPE_TAG);
    save_node_to_MySQL(mysqldb, 0, NODE_TYPE_TAG, "Tag001", "0410000000000001",  -757, 1769, 130, "4.11.6", 160, 1, 1);
    save_node_to_MySQL(mysqldb, 1, NODE_TYPE_TAG, "Tag002", "0410000000000002",  -752, 1647, 110, "4.11.6", 161, 1, 1);
    save_node_to_MySQL(mysqldb, 2, NODE_TYPE_TAG, "Tag003", "0410000000000003",  -270, 1727, 120, "4.11.6", 162, 1, 1);
    save_node_to_MySQL(mysqldb, 3, NODE_TYPE_TAG, "Tag004", "0410000000000004",  -514, 1738, 110, "4.11.6", 163, 1, 1);
    save_node_to_MySQL(mysqldb, 4, NODE_TYPE_TAG, "Tag005", "0410000000000005",  -503, 1600, 100, "4.11.6", 164, 1, 1);
    save_node_to_MySQL(mysqldb, 5, NODE_TYPE_TAG, "Tag006", "0410000000000006",  -190, 1594, 70, "4.11.6", 165, 1, 1);
    save_node_to_MySQL(mysqldb, 6, NODE_TYPE_TAG, "Tag007", "0410000000000007",    47, 1552, 95, "4.11.6", 166, 1, 1);
    save_node_to_MySQL(mysqldb, 7, NODE_TYPE_TAG, "Tag008", "0410000000000008",   158, 1647, 90, "4.11.6", 167, 1, 1);
    save_node_to_MySQL(mysqldb, 8, NODE_TYPE_TAG, "Tag009", "0410000000000009",   418, 1404, 110, "4.11.6", 168, 1, 1);
    save_node_to_MySQL(mysqldb, 9, NODE_TYPE_TAG, "Tag010", "0410000000000010",   339, 1600, 120, "4.11.6", 169, 1, 1);

    // save_node_to_MySQL(mysqldb, 0, NODE_TYPE_TAG, "Tag001", "0410000000000001",   794,  3703, 130, "4.11.6", 160, 1, 1);
    // save_node_to_MySQL(mysqldb, 1, NODE_TYPE_TAG, "Tag002", "0410000000000002",   688,  3476, 110, "4.11.6", 161, 1, 1);
    // save_node_to_MySQL(mysqldb, 2, NODE_TYPE_TAG, "Tag003", "0410000000000003",  1261,  3428, 120, "4.11.6", 162, 1, 1);
    // save_node_to_MySQL(mysqldb, 3, NODE_TYPE_TAG, "Tag004", "0410000000000004",   805,  3401, 110, "4.11.6", 163, 1, 1);
    // save_node_to_MySQL(mysqldb, 4, NODE_TYPE_TAG, "Tag005", "0410000000000005",  1059,  3343, 100, "4.11.6", 164, 1, 1);
    // save_node_to_MySQL(mysqldb, 5, NODE_TYPE_TAG, "Tag006", "0410000000000006",  1367,  3295, 70, "4.11.6", 165, 1, 1);
    // save_node_to_MySQL(mysqldb, 6, NODE_TYPE_TAG, "Tag007", "0410000000000007",  1563,  3285, 95, "4.11.6", 166, 1, 1);
    // save_node_to_MySQL(mysqldb, 7, NODE_TYPE_TAG, "Tag008", "0410000000000008",  1647,  3571, 90, "4.11.6", 167, 1, 1);
    // save_node_to_MySQL(mysqldb, 8, NODE_TYPE_TAG, "Tag009", "0410000000000009",  1785,  3354, 110, "4.11.6", 168, 1, 1);
    // save_node_to_MySQL(mysqldb, 9, NODE_TYPE_TAG, "Tag010", "0410000000000010",  1844,  3518, 120, "4.11.6", 169, 1, 1);

    // Anchor
    mysqldb->delete_all_node(NODE_TYPE_ANCHOR);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An001", "0320000000000001", -2829, -158, 320, "3.11.6", 200, 1, 1);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_ANCHOR, "An002", "0320000000000002", -2840,  763, 320, "3.11.6", 201, 1, 1);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_ANCHOR, "An003", "0320000000000003", -1536, -233, 320, "3.11.6", 202, 1, 1);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_ANCHOR, "An004", "0320000000000004", -2098,  392, 320, "3.11.6", 203, 1, 1);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_ANCHOR, "An005", "0320000000000005", -2246, 1515, 320, "3.11.6", 204, 1, 1);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_ANCHOR, "An006", "0320000000000006", -2024,  953, 320, "3.11.6", 205, 1, 1);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_ANCHOR, "An007", "0320000000000007", -1695, 1748, 320, "3.11.6", 206, 1, 1);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_ANCHOR, "An008", "0320000000000008", -1473, 1409, 320, "3.11.6", 207, 1, 1);
    save_node_to_MySQL(mysqldb,  8, NODE_TYPE_ANCHOR, "An009", "0320000000000009", -1218, 1854, 320, "3.11.6", 208, 1, 1);
    save_node_to_MySQL(mysqldb,  9, NODE_TYPE_ANCHOR, "An010", "0320000000000010",  -625, 1589, 320, "3.11.6", 209, 1, 1);
    save_node_to_MySQL(mysqldb, 10, NODE_TYPE_ANCHOR, "An011", "0320000000000011",    74, 1494, 320, "3.11.6", 210, 1, 1);
    save_node_to_MySQL(mysqldb, 11, NODE_TYPE_ANCHOR, "An012", "0320000000000012",   264, 1780, 320, "3.11.6", 211, 1, 1);
    save_node_to_MySQL(mysqldb, 12, NODE_TYPE_ANCHOR, "An013", "0320000000000013",  1091, 1451, 320, "3.11.6", 212, 1, 1);
    save_node_to_MySQL(mysqldb, 13, NODE_TYPE_ANCHOR, "An014", "0320000000000014",   339,  540, 320, "3.11.6", 213, 1, 1);
    save_node_to_MySQL(mysqldb, 14, NODE_TYPE_ANCHOR, "An015", "0320000000000015",  1759,  678, 320, "3.11.6", 214, 1, 1);
    save_node_to_MySQL(mysqldb, 15, NODE_TYPE_ANCHOR, "An016", "0320000000000016",  -264,   84, 320, "3.11.6", 215, 1, 1);
    save_node_to_MySQL(mysqldb, 16, NODE_TYPE_ANCHOR, "An017", "0320000000000017",  1727, -211, 320, "3.11.6", 216, 1, 1);
    save_node_to_MySQL(mysqldb, 17, NODE_TYPE_ANCHOR, "An018", "0320000000000018",   741, 1049, 320, "3.11.6", 217, 1, 1);
    save_node_to_MySQL(mysqldb, 18, NODE_TYPE_ANCHOR, "An019", "0320000000000019",  1049, -932, 320, "3.11.6", 218, 1, 1);
    save_node_to_MySQL(mysqldb, 19, NODE_TYPE_ANCHOR, "An020", "0320000000000020",  1197,  307, 320, "3.11.6", 219, 1, 1);
    save_node_to_MySQL(mysqldb, 20, NODE_TYPE_ANCHOR, "An021", "0320000000000021",   445,-1314, 320, "3.11.6", 220, 1, 1);
    save_node_to_MySQL(mysqldb, 21, NODE_TYPE_ANCHOR, "An022", "0320000000000022", -2342, -710, 320, "3.11.6", 221, 1, 1);
    save_node_to_MySQL(mysqldb, 22, NODE_TYPE_ANCHOR, "An023", "0320000000000023", -1685,-1112, 320, "3.11.6", 222, 1, 1);

    // save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An001", "0320000000000001", -1356,  137, 320, "3.11.6", 200, 1, 1);
    // save_node_to_MySQL(mysqldb,  1, NODE_TYPE_ANCHOR, "An002", "0320000000000002", -1642, 1213, 320, "3.11.6", 201, 1, 1);
    // save_node_to_MySQL(mysqldb,  2, NODE_TYPE_ANCHOR, "An003", "0320000000000003",   -90,  143, 320, "3.11.6", 202, 1, 1);
    // save_node_to_MySQL(mysqldb,  3, NODE_TYPE_ANCHOR, "An004", "0320000000000004",  -614, 1075, 320, "3.11.6", 203, 1, 1);
    // save_node_to_MySQL(mysqldb,  4, NODE_TYPE_ANCHOR, "An005", "0320000000000005", -1515, 2368, 320, "3.11.6", 204, 1, 1);
    // save_node_to_MySQL(mysqldb,  5, NODE_TYPE_ANCHOR, "An006", "0320000000000006",  -556, 2040, 320, "3.11.6", 205, 1, 1);
    // save_node_to_MySQL(mysqldb,  6, NODE_TYPE_ANCHOR, "An007", "0320000000000007",  -831, 3280, 320, "3.11.6", 206, 1, 1);
    // save_node_to_MySQL(mysqldb,  7, NODE_TYPE_ANCHOR, "An008", "0320000000000008",    37, 2707, 320, "3.11.6", 207, 1, 1);
    // save_node_to_MySQL(mysqldb,  8, NODE_TYPE_ANCHOR, "An009", "0320000000000009",    79, 3857, 320, "3.11.6", 208, 1, 1);
    // save_node_to_MySQL(mysqldb,  9, NODE_TYPE_ANCHOR, "An010", "0320000000000010",   667, 2940, 320, "3.11.6", 209, 1, 1);
    // save_node_to_MySQL(mysqldb, 10, NODE_TYPE_ANCHOR, "An011", "0320000000000011",  1573, 2781, 320, "3.11.6", 210, 1, 1);
    // save_node_to_MySQL(mysqldb, 11, NODE_TYPE_ANCHOR, "An012", "0320000000000012",  2130, 3677, 320, "3.11.6", 211, 1, 1);
    // save_node_to_MySQL(mysqldb, 12, NODE_TYPE_ANCHOR, "An013", "0320000000000013",  2988, 2861, 320, "3.11.6", 212, 1, 1);
    // save_node_to_MySQL(mysqldb, 13, NODE_TYPE_ANCHOR, "An014", "0320000000000014",  2257, 2214, 320, "3.11.6", 213, 1, 1);
    // save_node_to_MySQL(mysqldb, 14, NODE_TYPE_ANCHOR, "An015", "0320000000000015",  3476, 1451, 320, "3.11.6", 214, 1, 1);
    // save_node_to_MySQL(mysqldb, 15, NODE_TYPE_ANCHOR, "An016", "0320000000000016",  2479, 1441, 320, "3.11.6", 215, 1, 1);
    // save_node_to_MySQL(mysqldb, 16, NODE_TYPE_ANCHOR, "An017", "0320000000000017",  3264,  413, 320, "3.11.6", 216, 1, 1);
    // save_node_to_MySQL(mysqldb, 17, NODE_TYPE_ANCHOR, "An018", "0320000000000018",  2031, 1155, 320, "3.11.6", 217, 1, 1);
    // save_node_to_MySQL(mysqldb, 18, NODE_TYPE_ANCHOR, "An019", "0320000000000019",  2732, -275, 320, "3.11.6", 218, 1, 1);
    // save_node_to_MySQL(mysqldb, 19, NODE_TYPE_ANCHOR, "An020", "0320000000000020",  1197,  307, 320, "3.11.6", 219, 1, 1);
    // save_node_to_MySQL(mysqldb, 20, NODE_TYPE_ANCHOR, "An021", "0320000000000021",  2013, -943, 320, "3.11.6", 220, 1, 1);
    // save_node_to_MySQL(mysqldb, 21, NODE_TYPE_ANCHOR, "An022", "0320000000000022",  -699, -593, 320, "3.11.6", 221, 1, 1);
    // save_node_to_MySQL(mysqldb, 22, NODE_TYPE_ANCHOR, "An023", "0320000000000023",  211, -1028, 320, "3.11.6", 222, 1, 1);


    // Coordinator
    mysqldb->delete_all_node(NODE_TYPE_COORD);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord001", "3130000000000001", -784, -498, 220, "32.11.5", 500, 1, 1);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_COORD, "Coord002", "3130000000000002",-1950,   95, 220, "32.11.5", 500, 1, 1);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_COORD, "Coord003", "3130000000000003",-1716, 1218, 220, "32.11.5", 500, 1, 1);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_COORD, "Coord004", "3130000000000004", -275, 1568, 220, "32.11.5", 500, 1, 1);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_COORD, "Coord005", "3130000000000005",  964,  847, 220, "32.11.5", 500, 1, 1);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_COORD, "Coord006", "3130000000000006",  328, -423, 220, "32.11.5", 500, 1, 1);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_COORD, "Coord007", "3130000000000007",  847,   31, 220, "32.11.5", 500, 1, 1);

    // save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord001", "3130000000000001",  508, -349, 220, "32.11.5", 500, 1, 1);
    // save_node_to_MySQL(mysqldb,  1, NODE_TYPE_COORD, "Coord002", "3130000000000002", -625,  635, 220, "32.11.5", 500, 1, 1);
    // save_node_to_MySQL(mysqldb,  2, NODE_TYPE_COORD, "Coord003", "3130000000000003", -551, 2628, 220, "32.11.5", 500, 1, 1);
    // save_node_to_MySQL(mysqldb,  3, NODE_TYPE_COORD, "Coord004", "3130000000000004", 1176, 3200, 220, "32.11.5", 500, 1, 1);
    // save_node_to_MySQL(mysqldb,  4, NODE_TYPE_COORD, "Coord005", "3130000000000005", 2405, 2808, 220, "32.11.5", 500, 1, 1);
    // save_node_to_MySQL(mysqldb,  5, NODE_TYPE_COORD, "Coord006", "3130000000000006", 2363,  646, 220, "32.11.5", 500, 1, 1);
    // save_node_to_MySQL(mysqldb,  6, NODE_TYPE_COORD, "Coord007", "3130000000000007", 1621, -243, 220, "32.11.5", 500, 1, 1);

    int project_id = 1;
    // create project map
    // mysqldb->write_map(1, project_id, "map", "3127.708809376849", "1547.9575365340313", "26.084469514364343", "100000", "/upload/crec2.png", "1200", "848");
    //mysqldb->write_map(1, 1, "map", "2295.7727092969417", "1335.9992944754563", "52.421326267464195", "100000", "/upload/crec2d.png", "3000", "3000");

    // mysqldb->write_maparea("area7481", project_id, "集合區", "{\"type\":\"Feature\",\"properties\":{\"style\":{\"color\":\"#8b95aa\",\"opacity\":\"0.8\",\"weight\":4,\"fillColor\":\"#83d234\",\"fillOpacity\":\"0.50\"},\"areaid\":\"area7481\",\"areaname\":\"集合區\"},\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[-0.00008392333984375,0.0001697540283203125],[-0.000110626220703125,0.00016689300537109375],[-0.00009632110595703125,0.000133514404296875],[-0.000049591064453125,0.00013637542724609375],[-0.0000095367431640625,0.00012683868408203125],[0.000026702880859375,0.00011730194091796875],[0.00004673004150390625,0.00010585784912109375],[0.0000858306884765625,0.00013065338134765625],[0.00006008148193359375,0.00014591217041015625],[0.00003147125244140625,0.00015544891357421875],[0.000003814697265625,0.000164031982421875],[-0.000026702880859375,0.0001678466796875],[-0.00005817413330078125,0.0001697540283203125],[-0.00008392333984375,0.0001697540283203125]]]}}", 0);
    //
    // mysqldb->write_maparea("area2860", project_id, "危险区", "{\"type\":\"Feature\",\"properties\":{\"style\":{\"color\":\"#8b95aa\",\"opacity\":\"0.8\",\"weight\":4,\"fillColor\":\"#e02853\",\"fillOpacity\":\"0.96\"},\"areaid\":\"area2860\",\"areaname\":\"危险区\"},\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[[-0.00015783309936523438,0.00010442733764648438],[-0.00008249282836914062,0.00009059906005859375],[-0.00009632110595703125,0.00008106231689453125],[-0.00010347366333007812,0.00007295608520507812],[-0.00010585784912109375,0.00006103515625],[-0.00010251998901367188,0.000055789947509765625],[-0.0001010894775390625,0.000046253204345703125],[-0.00010061264038085938,0.0000457763671875],[-0.00018405914306640625,0.00002288818359375],[-0.00018787384033203125,0.000040531158447265625],[-0.000186920166015625,0.00006198883056640625],[-0.00018453598022460938,0.00007677078247070312],[-0.00017833709716796875,0.00008869171142578125],[-0.00015783309936523438,0.00010442733764648438]]]}}", 0);

    mysqldb->clear_all_event(project_id);
    mysqldb->write_event(project_id, 1, "Tag001", "Enter", "area7481", "0");
    mysqldb->write_event(project_id, 2, "Tag001", "Leave", "area7481", "0");
    mysqldb->write_event(project_id, 3, "Tag001", "Enter", "area2860", "0");
    mysqldb->write_event(project_id, 4, "Tag001", "Leave", "area2860", "0");

    // mysqldb->write_config(project_id, "1F", "1F", "121.538550425548",  "25.0402272235039", "90.00000000", 1, 1);

    delete mysqldb;
}

void save_KAOOFFICE_node_to_MySQL()
{
    MySQLDBAccess * mysqldb = new MySQLDBAccess(1);

    // Tag
    //mysqldb->delete_all_node(NODE_TYPE_TAG);
   // save_node_to_MySQL(mysqldb,  0, NODE_TYPE_TAG, "Tag026", "0410000000000026",  0, 0, 130, "4.11.6", 160, 1, 4);
   // save_node_to_MySQL(mysqldb,  1, NODE_TYPE_TAG, "Tag340", "0410000000000340",  0, 0, 131, "4.11.6", 161, 1, 4);


//    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_TAG, "Tag006", "0410000000000006",  0, 0, 130, "4.11.6", 160, 1, 4);
//    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_TAG, "Tag007", "0410000000000007",  0, 0, 131, "4.11.6", 161, 1, 4);
//    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_TAG, "Tag008", "0410000000000008",  0, 0, 132, "4.11.6", 162, 1, 4);
//    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_TAG, "Tag009", "0410000000000009",  0, 0, 133, "4.11.6", 163, 1, 4);
//    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_TAG, "Tag010", "0410000000000010",  0, 0, 134, "4.11.6", 164, 1, 4);
//    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_TAG, "Tag011", "0410000000000011",  0, 0, 135, "4.11.6", 165, 1, 4);
//    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_TAG, "Tag012", "0410000000000012",  0, 0, 136, "4.11.6", 166, 1, 4);
//    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_TAG, "Tag013", "0410000000000013",  0, 0, 137, "4.11.6", 167, 1, 4);
//    save_node_to_MySQL(mysqldb,  8, NODE_TYPE_TAG, "Tag014", "0410000000000014",  0, 0, 138, "4.11.6", 168, 1, 4);
//    save_node_to_MySQL(mysqldb,  9, NODE_TYPE_TAG, "Tag015", "0410000000000015",  0, 0, 139, "4.11.6", 169, 1, 4);
//    save_node_to_MySQL(mysqldb, 10, NODE_TYPE_TAG, "Tag016", "0410000000000016",  0, 0, 140, "4.11.6", 170, 1, 4);

    // int TagCount = 100;
    // for(int i=0;i<TagCount;i++)
    // {
    //     char TagId[32];
    //     char TagMAC[32];
    //     sprintf(TagId, "Tag%03d", i);
    //     sprintf(TagMAC, "0410000000000%03d", i+5);
    //
    //     save_node_to_MySQL(mysqldb,  i, NODE_TYPE_TAG, TagId, TagMAC,  0, 0, 130 + i, "4.11.6", 160 + i, 1, 4);
    // }


    // Anchor
    //mysqldb->delete_all_node(NODE_TYPE_ANCHOR);
    // save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An011", "3131000000000011",   0,   0, 165, "32.11.5", 200, 1, 4);
    // save_node_to_MySQL(mysqldb,  1, NODE_TYPE_ANCHOR, "An116", "0320000000000116", 500,   0, 140,  "3.11.6", 201, 1, 4);
    // save_node_to_MySQL(mysqldb,  2, NODE_TYPE_ANCHOR, "An114", "0320000000000114", 500, 500, 110,  "3.11.6", 202, 1, 4);
    // save_node_to_MySQL(mysqldb,  3, NODE_TYPE_ANCHOR, "An117", "0320000000000117",   0, 500, 250,  "3.11.6", 203, 1, 4);

    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_ANCHOR, "An0020", "0320000000000020",  318, 2299, 0, "3.11.5", 200, 1, 1);
    save_node_to_MySQL(mysqldb,  6, NODE_TYPE_ANCHOR, "An0086", "0320000000000086", 1545, 2299, 0, "3.11.6", 201, 1, 1);
    save_node_to_MySQL(mysqldb,  7, NODE_TYPE_ANCHOR, "An0097", "0320000000000097",   77, 1205, 0, "3.11.6", 202, 1, 1);
    save_node_to_MySQL(mysqldb,  8, NODE_TYPE_ANCHOR, "An0107", "0320000000000107", 1791, 1218, 0, "3.11.6", 203, 1, 1);
    save_node_to_MySQL(mysqldb,  9, NODE_TYPE_ANCHOR, "An0114", "0320000000000114",  306,  239, 0, "3.11.6", 203, 1, 1);
    save_node_to_MySQL(mysqldb, 10, NODE_TYPE_ANCHOR, "An0117", "0320000000000117", 1789,  235, 0, "3.11.6", 203, 1, 1);


    // Coordinator
    //mysqldb->delete_all_node(NODE_TYPE_COORD);
    //save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord011", "3130000000000011", 280, 40, 165, "32.11.5", 500, 1, 1);

    // Locator
    //mysqldb->delete_all_node(NODE_TYPE_LOCATOR);
    //save_node_to_MySQL(mysqldb,  0, NODE_TYPE_LOCATOR, "Locator001", "3140000000000001", "00000000000000fc", 100, 40, 100, "33.11.5", 500, 1, 4);


    // int project_id = 1;
    // // create project map
    // //mysqldb->write_map(1, project_id, "map", "0.0", "0.0", "56.68252181642107", "100000", "/upload/2dgrid.png", "600", "600");
    // mysqldb->write_map(1, project_id, "map", "358.6730877334951", "80.47789503161523", "62.60024579510519", "100000", "/upload/KaoOffice_grid.png", "600", "829");
    //
    // mysqldb->write_config(project_id, "125室", "高雄市左營區立文路77號4樓125室", "USER_ID", "PROJECT_ID", "vils-iot-datacenter.appspot.com", "80", "PROJECT_ACCESS_TOKEN", "OFF", 121.538550425548,  25.0402272235039, 90.00000000, 121.538550425548,  25.0402272235039, 0.00010000, 0.00010000, 1, 4);

    delete mysqldb;
}

void save_AUO_node_to_MySQL()
{
    MySQLDBAccess * mysqldb = new MySQLDBAccess(1);

    // Tag
    mysqldb->delete_all_node(NODE_TYPE_TAG);
    save_node_to_MySQL(mysqldb,  0, NODE_TYPE_TAG, "Tag126", "0410000000000126",  0, 0, 0, "4.11.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb,  1, NODE_TYPE_TAG, "Tag013", "0410000000000013",  0, 0, 0, "4.11.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb,  2, NODE_TYPE_TAG, "Tag015", "0410000000000015",  0, 0, 0, "4.11.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb,  3, NODE_TYPE_TAG, "Tag001", "0410000000000001",  0, 0, 0, "4.11.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb,  4, NODE_TYPE_TAG, "Tag007", "0410000000000007",  0, 0, 0, "4.11.6", 0, 1, 1);
    save_node_to_MySQL(mysqldb,  5, NODE_TYPE_TAG, "Tag008", "0310000000000008",  0, 0, 0, "4.11.6", 0, 1, 1);

    // Anchor
    // mysqldb->delete_all_node(NODE_TYPE_ANCHOR);
    // save_node_to_MySQL(mysqldb,  0, NODE_TYPE_ANCHOR, "An014", "0320000000000014",-490,20,254,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  1, NODE_TYPE_ANCHOR, "An030", "0320000000000030",-160,1020,254,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  2, NODE_TYPE_ANCHOR, "An037", "0320000000000037",-2251,198,254,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  3, NODE_TYPE_ANCHOR, "An051", "0320000000000051",-100,180,254,"4.12.2", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  4, NODE_TYPE_ANCHOR, "An057", "0320000000000057",1079,-812,254,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  5, NODE_TYPE_ANCHOR, "An058", "0320000000000058",-2300,1244,253,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  6, NODE_TYPE_ANCHOR, "An060", "0320000000000060",-1332,-368,250,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  7, NODE_TYPE_ANCHOR, "An061", "0320000000000061",-1383,1394,254,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  8, NODE_TYPE_ANCHOR, "An062", "0320000000000062",-355,-365,0,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  9, NODE_TYPE_ANCHOR, "An064", "0320000000000064",-2298,-615,252,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 10, NODE_TYPE_ANCHOR, "An077", "0320000000000077",164,113,254,"3.12.3", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 11, NODE_TYPE_ANCHOR, "An078", "0320000000000078",-748,52,254,"3.12.3", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 12, NODE_TYPE_ANCHOR, "An079", "0320000000000079",-850,672,254,"3.12.3", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 13, NODE_TYPE_ANCHOR, "An111", "0320000000000111",164,-937,254,"3.12.6", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 14, NODE_TYPE_ANCHOR, "An007", "3131000000000007",-3643,1785,254,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 15, NODE_TYPE_ANCHOR, "An011", "3131000000000011",-430,720,232,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 16, NODE_TYPE_ANCHOR, "An012", "3131000000000012",1087,79,254,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 17, NODE_TYPE_ANCHOR, "An015", "3131000000000015",-1468,291,230,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 18, NODE_TYPE_ANCHOR, "An032", "3131000000000032",-47,1372,230,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 19, NODE_TYPE_ANCHOR, "An054", "3131000000000054",-997,-60,254,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb, 20, NODE_TYPE_ANCHOR, "An118", "0320000000000118",0,0,0,"32.12.5", 0, -100, -100);
    // save_node_to_MySQL(mysqldb, 21, NODE_TYPE_ANCHOR, "An120", "0320000000000120",0,0,0,"32.12.5", 0, -100, -100);
    // save_node_to_MySQL(mysqldb, 22, NODE_TYPE_ANCHOR, "An121", "0320000000000121",0,0,0,"32.12.5", 0, -100, -100);
    // save_node_to_MySQL(mysqldb, 23, NODE_TYPE_ANCHOR, "An122", "0320000000000122",0,0,0,"32.12.5", 0, -100, -100);
    // save_node_to_MySQL(mysqldb, 24, NODE_TYPE_ANCHOR, "An016", "3131000000000016",0,0,0,"32.12.5", 0, -100, -100);
    //
    //
    // // Coordinator
    // mysqldb->delete_all_node(NODE_TYPE_COORD);
    // save_node_to_MySQL(mysqldb,  0, NODE_TYPE_COORD, "Coord007", "3130000000000007",-3643,1785,254,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  1, NODE_TYPE_COORD, "Coord011", "3130000000000011",-430,720,232,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  2, NODE_TYPE_COORD, "Coord012", "3130000000000012",1087,79,254,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  3, NODE_TYPE_COORD, "Coord015", "3130000000000015",-1468,291,230,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  4, NODE_TYPE_COORD, "Coord032", "3131000000000032",-47,1372,230,"32.12.5", 0, 1, 1);
    // save_node_to_MySQL(mysqldb,  5, NODE_TYPE_COORD, "Coord054", "3130000000000054",-997,-60,254,"32.12.5", 0, 1, 1);

    delete mysqldb;
}

// void update_test_node_to_MySQL()
// {
//     glDeviceManager->setTagAlive("0410000000000001", "4.2", "37.1");
//     glDeviceManager->setTagAlive("0410000000000002", "4.3", "37.3");
//     glDeviceManager->setTagAlive("0410000000000003", "4.1", "36.3");
//     glDeviceManager->setTagAlive("0410000000000004", "3.8", "36.8");
//     glDeviceManager->setTagAlive("0410000000000005", "4.2", "33.2");
//     glDeviceManager->setTagAlive("0410000000000006", "3.9", "34.7");
//     glDeviceManager->setTagAlive("0410000000000007", "4.0", "37.3");
//     glDeviceManager->setTagAlive("0410000000000008", "4.1", "36.1");
//     glDeviceManager->setTagAlive("0410000000000009", "4.1", "32.6");
//     glDeviceManager->setTagAlive("0410000000000010", "4.3", "34.3");
//
//
//     glDeviceManager->setCoordPanid("3130000000000001", "0328", "32.11.5");
//     glDeviceManager->setCoordPanid("3130000000000002", "0328", "32.11.5");
//     glDeviceManager->setCoordPanid("3130000000000003", "0328", "32.11.5");
//     glDeviceManager->setCoordPanid("3130000000000004", "0328", "32.11.5");
//     glDeviceManager->setCoordPanid("3130000000000005", "0328", "32.11.5");
//     glDeviceManager->setCoordPanid("3130000000000006", "0328", "32.11.5");
//     glDeviceManager->setCoordPanid("3130000000000007", "0328", "32.11.5");
//     //glDeviceManager->setCoordPanid("3130000000000008", "0328", "32.11.5");
//     //glDeviceManager->setCoordPanid("3130000000000009", "0328", "32.11.5");
//     //glDeviceManager->setCoordPanid("3130000000000010", "0328", "32.11.5");
//
//
//     glDeviceManager->setCoordinatorInfo("3130000000000001", "192.168.10.100", "vilscoord_1.2.28-UDP", "482", "0328");
//     glDeviceManager->setCoordinatorInfo("3130000000000002", "192.168.10.102", "vilscoord_1.2.28-UDP", "481", "0328");
//     glDeviceManager->setCoordinatorInfo("3130000000000003", "192.168.10.103", "vilscoord_1.2.28-UDP", "521", "0328");
//     glDeviceManager->setCoordinatorInfo("3130000000000004", "192.168.10.107", "vilscoord_1.2.28-UDP", "492", "0328");
//     glDeviceManager->setCoordinatorInfo("3130000000000005", "192.168.10.109", "vilscoord_1.2.28-UDP", "1023", "0328");
//     glDeviceManager->setCoordinatorInfo("3130000000000006", "192.168.10.111", "vilscoord_1.2.28-UDP", "1332", "0328");
//     glDeviceManager->setCoordinatorInfo("3130000000000007", "192.168.10.115", "vilscoord_1.2.28-UDP", "4092", "0328");
//     //glDeviceManager->setCoordinatorInfo("3130000000000008", "192.168.10.116", "vilscoord_1.2.28-UDP", "492", "0328");
//     //glDeviceManager->setCoordinatorInfo("3130000000000009", "192.168.10.118", "vilscoord_1.2.28-UDP", "495", "0328");
//     //glDeviceManager->setCoordinatorInfo("3130000000000010", "192.168.10.121", "vilscoord_1.2.28-UDP", "2381", "0328");
//
// }


void save_node_to_MySQL(MySQLDBAccess * mysqldb, int index, int nodetype,
        const char * nodeid, const char *macaddress,
        double posX, double posY, double posZ, const char * version,
        int tafid, int buildingid, int buildingfloor)
{
    NodeStruct * newNode = createBaseNodeStruct(index, nodetype,
        nodeid, macaddress, posX, posY, posZ, version, tafid, buildingid, buildingfloor);
    save_node_info_MySQL(newNode, mysqldb);
    delete newNode;
}

void save_node_to_MySQL(MySQLDBAccess * mysqldb, int index, int nodetype,
        const char * nodeid, const char *macaddress, const char *LoRA_macaddress,
        double posX, double posY, double posZ, const char * version,
        int tafid, int buildingid, int buildingfloor)
{
    NodeStruct * newNode = createBaseNodeStruct(index, nodetype,
        nodeid, macaddress, posX, posY, posZ, version, tafid, buildingid, buildingfloor);
    strcpy(newNode->LoRAMacaddress, LoRA_macaddress);
    save_node_info_MySQL(newNode, mysqldb);
    delete newNode;
}

void save_nodelist_info_MySQL(std::list<NodeStruct*> * nodeList, MySQLDBAccess * mysqldb)
{
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        save_node_info_MySQL((*node), mysqldb);
    }
}

void save_node_info_MySQL(NodeStruct * node, MySQLDBAccess * mysqldb)
{
    mysqldb->Transaction_write_node_info(node);
    // mysqldb->write_node_info(node->nodetype, node->macaddress, node->LoRAMacaddress, node->nodeid, node->tafid,
    //     node->buildingid, node->buildingfloor, node->nodename,
    //     node->index, (int)node->posX, (int)node->posY, (int)node->posZ, node->Longitude, node->Latitude,
    //     node->voltage, node->temperature, node->version, node->AreaId, node->AreaName);
}
