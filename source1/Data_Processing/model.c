#include "model.h"

const real params_inc[PARAM_SZ] = {4.5236539095640182495117188e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.8906197994947433471679688e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.1027496084570884704589844e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.1816855520009994506835938e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.8513863906264305114746094e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.5465827435255050659179688e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.9103908911347389221191406e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.0141055807471275329589844e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.1111882999539375305175781e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.5746861472725868225097656e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.2722058221697807312011719e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.8370609730482101440429688e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.0953189358115196228027344e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.7167538180947303771972656e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.1991010978817939758300781e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.4867624267935752868652344e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 4.0640637278556823730468750e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -6.2471505254507064819335938e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 5.7650566101074218750000000e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -9.5846071839332580566406250e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.2540268898010253906250000e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.0969670116901397705078125e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 5.1007699966430664062500000e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.0717704445123672485351562e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 5.5884059518575668334960938e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 7.8166658058762550354003906e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.4297619238495826721191406e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -7.1913180872797966003417969e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.4256756305694580078125000e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.4930642209947109222412109e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.5311541967093944549560547e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -6.2280283309519290924072266e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.7031250521540641784667969e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -8.7017947807908058166503906e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 7.5282650068402290344238281e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.8250641878694295883178711e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.2975839674472808837890625e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.6182804722338914871215820e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 4.7864927910268306732177734e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -4.0612695738673210144042969e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 6.0770334675908088684082031e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.9723865445703268051147461e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.1231167484074831008911133e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.4926615403965115547180176e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.0730752041563391685485840e-04};
const real params_dec[PARAM_SZ] = {3.9244759827852249145507812e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.6207318305969238281250000e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.0309960469603538513183594e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.3297493569552898406982422e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.2386753931641578674316406e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.2984644174575805664062500e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.5728903710842132568359375e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.1862333416938781738281250e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 5.0609763711690902709960938e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -4.8739027231931686401367188e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.8765540570020675659179688e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -7.3735550045967102050781250e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.2794870138168334960937500e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -9.5981158316135406494140625e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 9.4949901103973388671875000e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -9.7282037138938903808593750e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 5.8362994343042373657226562e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.3814296349883079528808594e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 6.3212200999259948730468750e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.0462686419486999511718750e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.0959974676370620727539062e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -8.8156826794147491455078125e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.2601414322853088378906250e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 2.0287121832370758056640625e-01, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.7334438413381576538085938e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -4.4815201312303543090820312e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 5.2392367273569107055664062e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.1655820310115814208984375e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.9533978775143623352050781e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.6536939665675163269042969e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.6434661597013473510742188e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -3.0931554734706878662109375e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.2923060916364192962646484e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.7225680649280548095703125e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.3616404980421066284179688e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.1820871159434318542480469e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -2.8565113898366689682006836e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.3462659902870655059814453e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 6.8866442888975143432617188e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.5364477876573801040649414e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 1.7050872556865215301513672e-03, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -1.9833179190754890441894531e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 3.7698425352573394775390625e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, -7.3093354701995849609375000e-02, 0.0000000000000000000000000e+00, 0.0000000000000000000000000e+00, 9.0892829000949859619140625e-02};

const int params_inc_ind[PARAM_SZ] = {2, 3, 5, 6, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 20, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 37, 38, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 52, 53, 54, 56, 57, 59, 60, 61, 62, 63, 65, 66, 67, 68, 69, 71, 72, 73, 74, 75, 77, 78, 80, 81, 82, 84, 85, 86, 87, 88, 90, 91, 93, 94, 96, 97, 98, 99, 100, 102, 103, 105, 106, 107, 109, 110, 111, 112, 113, 115, 116, 117, 118, 119, 121, 122, 123, 124, 125, 127, 128, 130, 131, 133, 134, 135, 136, 137, 139, 140, 141, 142, 143, 145, 146, 147, 149, 150, 152, 153, 154, 155, 156, 158, 159, 160, 161, 162, 164, 165, 166, 167, 168, 170, 171, 173, 174, 175, 177, 178, 179, 180, 181, 183, 184, 186, 187, 189, 190, 191, 192, 193, 195, 196, 198, 199, 200, 197, 163, 194, 169, 172, 188, 182, 176, 185, 144, 126, 157, 114, 151, 13, 64, 45, 70, 132, 138, 148, 51, 19, 26, 32, 7, 120, 22, 39, 58, 104, 55, 36, 129, 4, 76, 1, 108, 83, 79, 89, 95, 92, 101};

const int params_dec_ind[PARAM_SZ] = {2, 3, 5, 6, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 20, 21, 23, 24, 25, 27, 28, 29, 30, 31, 33, 34, 35, 37, 38, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 52, 53, 55, 56, 58, 59, 60, 61, 62, 64, 65, 67, 68, 69, 71, 72, 73, 74, 75, 77, 78, 79, 80, 81, 83, 84, 85, 87, 88, 90, 91, 92, 93, 94, 96, 97, 99, 100, 102, 103, 104, 105, 106, 108, 109, 110, 111, 112, 114, 115, 117, 118, 119, 121, 122, 123, 124, 125, 127, 128, 129, 130, 131, 133, 134, 135, 137, 138, 140, 141, 142, 143, 144, 146, 147, 148, 149, 150, 152, 153, 155, 156, 158, 159, 160, 161, 162, 164, 165, 166, 167, 168, 170, 171, 172, 174, 175, 177, 178, 179, 180, 181, 183, 184, 185, 186, 187, 189, 190, 191, 192, 193, 195, 196, 198, 199, 176, 182, 163, 173, 145, 13, 169, 126, 188, 7, 120, 157, 19, 76, 26, 132, 151, 45, 139, 32, 22, 154, 4, 136, 107, 194, 1, 113, 39, 36, 116, 70, 82, 197, 51, 95, 200, 63, 57, 66, 86, 89, 98, 54, 101};

const BiquadFilter filters[FILTER_SZ] = { 
{ .b0 = 2.024853e-08, .b1 = 4.049706e-08, .b2 = 2.024853e-08, .a1 = -1.776774e+00, .a2 = 7.926974e-01 },{ .b0 = 1.000000e+00, .b1 = 2.000000e+00, .b2 = 1.000000e+00, .a1 = -1.781984e+00, .a2 = 8.225319e-01 },{ .b0 = 1.000000e+00, .b1 = 2.000000e+00, .b2 = 1.000000e+00, .a1 = -1.802155e+00, .a2 = 8.789016e-01 },{ .b0 = 1.000000e+00, .b1 = 2.000000e+00, .b2 = 1.000000e+00, .a1 = -1.850813e+00, .a2 = 9.560229e-01 },};