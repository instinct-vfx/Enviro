// TODO: Add license/disclaimer

static const char* const CLASS = "Enviro";

static const char* const HELP =
"Enviro is a public exercise towards a reusable example for proper channel"
"handling in Iops";

#include "DDImage/Iop.h"
#include "DDImage/Row.h"
#include "DDImage/Tile.h"
#include "DDImage/Knobs.h"

using namespace DD::Image;

class Enviro : public Iop
{
	// Parameters (Knobs)
	Channel Normals[3];
	Channel Position[3];
	Channel Output[3];

public:
	Enviro(Node*);

	void _validate(bool);
	void _request(int, int, int, int, ChannelMask, int);
	void engine(int y, int x, int r, ChannelMask, Row &);

	int minimum_inputs() const;
	const char* input_label(int n, char *buf) const;
	virtual void knobs(Knob_Callback);

	const char* Class() const { return CLASS; }
	const char* node_help() const { return HELP; }
	static const Description d;

	// You may need to implement the destructor if you allocate memory:
	//~Enviro();
};

Enviro::Enviro(Node* node) : Iop(node)
{
	//Initialize parameters
	Normals[0] = Normals[1] = Normals[2] = Chan_Black;
	Position[0] = Position[1] = Position[2] = Chan_Black;
	Output[0] = Output[1] = Output[2] = Chan_Black;
}

int Enviro::minimum_inputs() const
{
	return 2;
}

const char* Enviro::input_label(int n, char *buf) const
{
	switch (n)
	{
	case 0: return "Input";
	case 1: return "Environment";
	case 2: return "Camera";
	}
	return buf;
}

void Enviro::knobs(Knob_Callback f)
{
	//Knob to select the normals layer
	Input_Channel_knob(f, Normals, 3, 0, "Normals");
	//Knob to select the position layer
	Input_Channel_knob(f, Position, 3, 0, "Position");
	//Knob to select the output layer
	Channel_knob(f, Output, 3, 0, "Output");

}

void Enviro::_validate(bool for_real)
{
	//TODO: Make sure we get the channels ans sizes produced from our inputs
	//TODO: Get the cameradata here
	copy_info();
	set_out_channels(Mask_All);
}

void Enviro::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
	//TODO: Make sure we request all channels that are needed. This needs to include
	//the channels that we are actually using (normals, position and the environment map)
	input0().request(x, y, r, t, channels, count);
}

void Enviro::engine(int y, int x, int r, ChannelMask channels, Row& out)
{

	//TODO: copy all channels we don't need or touch directly to output

	//TODO: get input pixels from input0 channels in &normals and &position into
	//local vars to make sure optimizer can identify them as static. This should
	//be prepared to enable getting surrounding pixels for multisampling and later
	//on glossy reflections.

	//TODO: Lock the whole environment 

	//TODO: Calculate coordinate in Environment. Get the color at the coordinate.
	//TODO: Write resulting color into the layer set in &output

	ChannelSet input;
	input = ChannelSet(Normals,3) + ChannelSet(Position, 3);
	//input += Position;


    // define an input row as it's faster then per pixel access
    Row in(x, y);
    in.get(input0(), y, x, r, input);

    float* TOr = out.writable(Output[0]);
    float* TOg = out.writable(Output[1]);
    float* TOb = out.writable(Output[2]);

    for (int X = x; X < r; ++X) {
		double nX = in[Normals[0]][X];
		double nY = in[Normals[1]][X];
		double nZ = in[Normals[2]][X];

        *TOr++ = nX*0.5f;
        *TOg++ = nY*0.5f;
        *TOb++ = nZ*0.5f;                                                
        
	}
}

static Iop* Enviro_c(Node* node) { return new Enviro(node); }
const Iop::Description Enviro::d(CLASS, "Filter/Enviro", Enviro_c);