#pragma once
// ============================================================
// PixelArt.hpp
// Pixel-art bitmaps for the hearts and the characters
// ============================================================


// Each character in a bitmap is one "pixel" (a tiny square)
const char* HEART_BITMAP[] = {
	".XX.XX.",
	"XhXXXXX",
	"XXXXXXX",
	".XXXXX.",
	"..XXX..",
	"...X..."
};
const int HEART_W = 7, HEART_H = 6;

// H = hair, S = skin, P = top, B = bottom
const char* CHARACTER_BITMAP[] = {
	"..HH..",
	".HSSH.",
	".SSSS.",
	"..SS..",
	".PPPP.",
	"SPPPPS",
	".BBBB.",
	".B..B."
};
// Girl version: long hair down to the shoulders and a flared skirt
// (same size as CHARACTER_BITMAP, same letters)
const char* GIRL_BITMAP[] = {
	"..HH..",
	".HHHH.",
	"HHSSHH",
	"H.SS.H",
	".PPPP.",
	"SPPPPS",
	"BBBBBB",
	".S..S."
};
const int CHAR_W = 6, CHAR_H = 8;
