// TODO: Add disclaimer

static const char* const CLASS = "Enviro";

static const char* const HELP =
"Enviro is a public exercise towards a reusable example for proper"
"channel handling in Iops";

#include "DDImage/Iop.h"
#include "DDImage/Row.h"
#include "DDImage/Tile.h"
#include "DDImage/Knobs.h"

using namespace DD::Image;

class Enviro : public Iop
{
	// Parameters
	double width, height;
	Channel Normals[3];
	Channel Position[3];

public:
	Enviro(Node*);
	void _validate(bool);
	void _request(int, int, int, int, ChannelMask, int);
	void engine(int y, int x, int r, ChannelMask, Row &);
	virtual void knobs(Knob_Callback);
	const char* Class() const { return CLASS; }
	const char* node_help() const { return HELP; }
	static const Description d;

	// You may need to implement the destructor if you allocate memory:
	//~Enviro();
};

Enviro::Enviro(Node* node) : Iop(node)
{
	width = 10;
	height = 10;
	Normals[0] = Normals[1] = Normals[2] = Chan_Black;
	Position[0] = Position[1] = Position[2] = Chan_Black;
}

void Enviro::knobs(Knob_Callback f)
{
	WH_knob(f, &width, "size");
	Input_Channel_knob(f, Normals, 3, 0, "Normals");

	Input_Channel_knob(f, Position, 3, 0, "Position");

}

static Iop* Enviro_c(Node* node) { return new Enviro(node); }

const Iop::Description Enviro::d(CLASS, "Filter/Enviro", Enviro_c);

void Enviro::_validate(bool for_real)
{
	copy_info();
	set_out_channels(Mask_All);
}

void Enviro::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
	input0().request(x, y, r, t, channels, count);
}

void Enviro::engine(int y, int x, int r, ChannelMask channels, Row& out)
{
	// Figure out a rectangle we want from the input:
	int w = int(width);
	int h = int(height);
	int tx = x / w * w;
	int tr = (r + w - 1) / w * w;
	int ty = y / h * h;
	int tt = ty + h;

	// Lock an area into the cache:
	// This should be an interest!!
	Tile tile(input0(), tx, ty, tr, tt, channels);
	// You must always check for aborted after creating a tile. If the
	// operation was aborted, the tile contains bad data:
	if (Op::aborted())
		return;

	for (int Y = ty; Y < tt; Y++) {
		// Retrieve a row from the tile. This is much faster than random
		// access of the tile and should be used if at all possible. See
		// the documentation for Interest, Tile, at at() for other ways of
		// getting the data.
		Row in(tx, tr);
		in.get(input0(), Y, tx, tr, channels);
		// For each channel that is needed:
		foreach (z, channels) {
			// For each horizontal block:
			for (int X = tx; X < tr; X += w) {
				// add up all the incoming pixels:
				float sum = 0;
				for (int X1 = 0; X1 < w; X1++)
					sum += in[z][X + X1];
				// divide by the total size of a block:
				sum /= (w * h);
				// add it to the output pixels (for first row replace the output):
				int outX = X;
				int E = X + w;
				if (outX < x)
					outX = x;
				if (E > r)
					E = r;
				float* TO = out.writable(z) + outX;
				float* END = TO + (E - outX);
				if (Y == ty) {
					while (TO < END)
						*TO++ = sum;
				}
				else {
					while (TO < END)
						*TO++ += sum;
				}
			}
		}
	}
}