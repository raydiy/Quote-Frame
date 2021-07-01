#include <Arduino.h>

#include "Frames/frame1tl.h"
#include "Frames/frame1br.h"
#include "Frames/frame2tl.h"
#include "Frames/frame2br.h"
#include "Frames/frame3tl.h"
#include "Frames/frame3br.h"
#include "Frames/frame4tl.h"
#include "Frames/frame4br.h"

typedef struct {
    uint8_t* topLeftFrame;
    uint8_t* bottomRightFrame;
    int width;
    int height;
} Frame;

static const Frame FRAMES[] = {
    {(uint8_t  *)image_data_frame1tl, (uint8_t  *)image_data_frame1br, 171, 171},
    {(uint8_t  *)image_data_frame2tl, (uint8_t  *)image_data_frame2br, 319, 188},
    {(uint8_t  *)image_data_frame3tl, (uint8_t  *)image_data_frame3br, 227, 151},
    {(uint8_t  *)image_data_frame4tl, (uint8_t  *)image_data_frame4br, 279, 180}
};