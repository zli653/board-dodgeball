#include "words.h"


const uint8_t scoreBitmaps[] =
{
	0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //            #         #                                                                                                                                                                                
	0xE3, 0x80, 0x0C, 0x00, 0x00, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x80, 0x00, 0x00, 0x00, // ###   ###           ##                            ### #                                                             #### #                                          ### #                             
	0x41, 0x00, 0x04, 0x00, 0x00, 0x40, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xC0, 0x00, 0x00, 0x00, 0x10, 0x11, 0x80, 0x00, 0x00, 0x00, //  #     #             #                   #       #   ##                                                            #    ##                                 #       #   ##                             
	0x41, 0x00, 0x04, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x40, 0x00, 0x00, 0x00, 0x10, 0x10, 0x80, 0x00, 0x00, 0x00, //  #     #             #                   #       #    #                                                           #      #                                 #       #    #                             
	0x41, 0x11, 0xF5, 0x87, 0x0E, 0xF0, 0x40, 0x38, 0x71, 0x67, 0x10, 0x00, 0x00, 0x00, 0x20, 0x19, 0x96, 0xB3, 0x8B, 0x3C, 0x10, 0x0E, 0x1C, 0x59, 0xC4, //  #     #   #   ##### # ##    ###    ### ####     #        ###    ###   # ##  ###   #                              #        ##  ##  # ## # ##  ###   # ##  ####     #        ###    ###   # ##  ###   #
	0x7F, 0x32, 0x26, 0x48, 0x92, 0x40, 0x30, 0x44, 0x8B, 0x88, 0x80, 0x00, 0x00, 0x00, 0x20, 0x08, 0xB9, 0xC4, 0x5C, 0x90, 0x0C, 0x11, 0x22, 0xE2, 0x20, //  #######  ##  #   #  ##  #  #   #  #  #  #        ##     #   #  #   # ###   #   #                                 #         #   # ###  ###   #   # ###  #  #        ##     #   #  #   # ###   #   #   
	0x41, 0x12, 0x24, 0x4F, 0x90, 0x40, 0x0C, 0x40, 0x89, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x20, 0x08, 0x90, 0x87, 0xC8, 0x90, 0x03, 0x10, 0x22, 0x43, 0xE0, //  #     #   #  #   #  #   #  #####  #     #          ##   #      #   #  #    #####                                 #         #   #  #    #    #####  #   #  #          ##   #      #   #  #    #####   
	0x41, 0x12, 0x24, 0x48, 0x0C, 0x40, 0x02, 0x40, 0x89, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x90, 0x84, 0x08, 0x90, 0x00, 0x90, 0x22, 0x42, 0x00, //  #     #   #  #   #  #   #  #       ##   #            #  #      #   #  #    #                                     #         #   #  #    #    #      #   #  #            #  #      #   #  #    #       
	0x41, 0x11, 0xC4, 0x48, 0x02, 0x40, 0x42, 0x40, 0x89, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x48, 0x90, 0x84, 0x08, 0x90, 0x10, 0x90, 0x22, 0x42, 0x00, //  #     #   #   ###   #   #  #         #  #       #    #  #      #   #  #    #                                     #      #  #   #  #    #    #      #   #  #       #    #  #      #   #  #    #       
	0x41, 0x12, 0x04, 0x48, 0x92, 0x50, 0x62, 0x44, 0x89, 0x08, 0x80, 0x00, 0x00, 0x00, 0x10, 0x89, 0xD0, 0x84, 0x48, 0x94, 0x18, 0x91, 0x22, 0x42, 0x20, //  #     #   #  #      #   #  #   #  #  #  # #     ##   #  #   #  #   #  #    #   #                                  #    #   #  ### #    #    #   #  #   #  # #     ##   #  #   #  #   #  #    #   #   
	0xE3, 0xB9, 0xEE, 0xE7, 0x1C, 0x60, 0x5C, 0x38, 0x73, 0x87, 0x10, 0x00, 0x00, 0x00, 0x0F, 0x06, 0xB9, 0xC3, 0x9D, 0xD8, 0x17, 0x0E, 0x1C, 0xE1, 0xC4, // ###   ### ###  #### ### ###  ###   ###   ##      # ###    ###    ###  ###    ###   #                                ####     ## # ###  ###    ###  ### ### ##      # ###    ###    ###  ###    ###   #
	0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //               #    #                                                                                                                                                                                  
	0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //               #    #                                                                                                                                                                                  
	0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                ####                                                                                                                                                                                   
};

// Bitmap sizes for score
const uint8_t scoreWidthPixels = 200;
const uint8_t scoreHeightPixels = 14;

const uint8_t start_gameBitmaps[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                                          #                                                                                #                                               
	0xFC, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ######                                  ##                                                                               ##                                               
	0x42, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x02, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  #    #                                  #                               #                    #               #       #   #                                               
	0x42, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x02, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  #    #                                  #                               #                    #               #       #   #                                               
	0x42, 0x59, 0xC3, 0x8E, 0x03, 0x47, 0x3B, 0xBA, 0xC0, 0xF3, 0x80, 0x77, 0x9C, 0x5F, 0x87, 0xAC, 0x38, 0x07, 0xCE, 0x2C, 0xC3, 0x80, //  #    #  # ##  ###    ###   ###       ## #   ###  ### ### ### # ##      ####  ###        ### ####  ###   # ######    #### # ##    ###        #####  ###   # ##  ##    ### 
	0x42, 0xE2, 0x24, 0x92, 0x04, 0xC8, 0x91, 0x17, 0x20, 0x44, 0x40, 0x92, 0x22, 0xE2, 0x02, 0x32, 0x44, 0x08, 0x91, 0x73, 0x24, 0x40, //  #    # ###   #   #  #  #  #  #      #  ##  #   #  #   #   # ###  #      #   #   #      #  #  #   #   # ###   #       #   ##  #  #   #      #   #  #   # ###  ##  #  #   #
	0x7C, 0x43, 0xE4, 0x10, 0x04, 0x48, 0x91, 0x12, 0x20, 0x44, 0x40, 0x82, 0x02, 0x42, 0x02, 0x22, 0x7C, 0x08, 0x81, 0x22, 0x27, 0xC0, //  #####   #    #####  #     #         #   #  #   #  #   #   #  #   #      #   #   #      #     #       #  #    #       #   #   #  #####      #   #      #  #   #   #  #####
	0x40, 0x42, 0x03, 0x0C, 0x04, 0x48, 0x8A, 0xA2, 0x20, 0x44, 0x40, 0x62, 0x1E, 0x42, 0x02, 0x22, 0x40, 0x08, 0x8F, 0x22, 0x24, 0x00, //  #       #    #       ##    ##       #   #  #   #   # # # #   #   #      #   #   #       ##   #    ####  #    #       #   #   #  #          #   #   ####  #   #   #  #    
	0x40, 0x42, 0x00, 0x82, 0x04, 0x48, 0x8A, 0xA2, 0x20, 0x44, 0x40, 0x12, 0x22, 0x42, 0x02, 0x22, 0x40, 0x07, 0x11, 0x22, 0x24, 0x00, //  #       #    #         #     #      #   #  #   #   # # # #   #   #      #   #   #         #  #   #   #  #    #       #   #   #  #           ###   #   #  #   #   #  #    
	0x40, 0x42, 0x24, 0x92, 0x04, 0xE8, 0x8C, 0xC2, 0x20, 0x54, 0x40, 0x92, 0xA2, 0x42, 0x82, 0xA2, 0x44, 0x08, 0x11, 0x22, 0x24, 0x40, //  #       #    #   #  #  #  #  #      #  ### #   #   ##  ##    #   #      # # #   #      #  #  # # #   #  #    # #     # # #   #  #   #      #      #   #  #   #   #  #   #
	0xE0, 0xE1, 0xC7, 0x1C, 0x03, 0x47, 0x04, 0x47, 0x70, 0x63, 0x80, 0xE3, 0x1F, 0xE3, 0x03, 0x77, 0x38, 0x07, 0x8F, 0xF7, 0x73, 0x80, // ###     ###    ###   ###   ###        ## #   ###     #   #   ### ###     ##   ###       ###   ##   ########   ##      ## ### ###  ###        ####   ######## ### ###  ### 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, //                                                                                                                                             #    #                        
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, //                                                                                                                                             #    #                        
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, //                                                                                                                                              ####                         
};

// Bitmap sizes for start_game
const uint8_t start_gameWidthPixels = 176;
const uint8_t start_gameHeightPixels = 14;

uint8_t *numbers[10];

uint8_t _0Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x38, //   ###   
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x38, //   ###   
	0x00, //         
	0x00, //         
};


/* Bitmap sizes for _0 */
const uint8_t numbers_WidthPixels = 8;
const uint8_t numbers_HeightPixels = 14;




 uint8_t _1Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x10, //    #    
	0x30, //   ##    
	0x10, //    #    
	0x10, //    #    
	0x10, //    #    
	0x10, //    #    
	0x10, //    #    
	0x10, //    #    
	0x10, //    #    
	0x38, //   ###   
	0x00, //         
	0x00, //                
};


/* 
**  Image data for _2
*/

 uint8_t _2Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x38, //   ###   
	0x44, //  #   #  
	0x04, //      #  
	0x04, //      #  
	0x08, //     #   
	0x08, //     #   
	0x10, //    #    
	0x20, //   #     
	0x44, //  #   #  
	0x7C, //  #####  
	0x00, //         
	0x00, //         
};


/* 
**  Image data for _3
*/

 uint8_t _3Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x38, //   ###   
	0x44, //  #   #  
	0x04, //      #  
	0x08, //     #   
	0x18, //    ##   
	0x04, //      #  
	0x04, //      #  
	0x04, //      #  
	0x04, //      #  
	0x78, //  ####   
	0x00, //         
	0x00, //         
};

/* 
**  Image data for _4
*/

 uint8_t _4Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x04, //      #  
	0x0C, //     ##  
	0x14, //    # #  
	0x14, //    # #  
	0x24, //   #  #  
	0x44, //  #   #  
	0x7E, //  ###### 
	0x04, //      #  
	0x04, //      #  
	0x04, //      #  
	0x00, //         
	0x00, //         
};




/* 
**  Image data for _5
*/

 uint8_t _5Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x1C, //    ###  
	0x20, //   #     
	0x20, //   #     
	0x70, //  ###    
	0x08, //     #   
	0x04, //      #  
	0x04, //      #  
	0x04, //      #  
	0x08, //     #   
	0x70, //  ###    
	0x00, //         
	0x00, //         
};


/* 
**  Image data for _6
*/

 uint8_t _6Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x06, //      ## 
	0x08, //     #   
	0x10, //    #    
	0x2C, //   # ##  
	0x32, //   ##  # 
	0x22, //   #   # 
	0x22, //   #   # 
	0x22, //   #   # 
	0x22, //   #   # 
	0x1C, //    ###  
	0x00, //         
	0x00, //         
};



/* 
**  Image data for _7
*/

 uint8_t _7Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x7E, //  ###### 
	0x82, // #     # 
	0x04, //      #  
	0x04, //      #  
	0x04, //      #  
	0x08, //     #   
	0x08, //     #   
	0x08, //     #   
	0x10, //    #    
	0x10, //    #    
	0x00, //         
	0x00, //         
};


/* 
**  Image data for _8
*/

 uint8_t _8Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x38, //   ###   
	0x44, //  #   #  
	0x44, //  #   #  
	0x28, //   # #   
	0x10, //    #    
	0x28, //   # #   
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x38, //   ###   
	0x00, //         
	0x00, //         
};


/* 
**  Image data for _9
*/

 uint8_t _9Bitmaps[] =
{
	0x00, //         
	0x00, //         
	0x38, //   ###   
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x44, //  #   #  
	0x3C, //   ####  
	0x04, //      #  
	0x08, //     #   
	0x10, //    #    
	0x60, //  ##     
	0x00, //         
	0x00, //         
};

void prepare_array(void){
	numbers[0] = _0Bitmaps;
	numbers[1] = _1Bitmaps;
	numbers[2] = _2Bitmaps;
	numbers[3] = _3Bitmaps;
	numbers[4] = _4Bitmaps;
	numbers[5] = _5Bitmaps;
	numbers[6] = _6Bitmaps;
	numbers[7] = _7Bitmaps;
	numbers[8] = _8Bitmaps;
	numbers[9] = _9Bitmaps;
}
