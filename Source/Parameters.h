struct ParamInfo
{
	const char* id;
	const char* name;
	float minValue;
	float maxValue;
	float defaultValue;
};
static constexpr ParamInfo paramList[] =
{
	{ "oct1Vol", "O1 Vol", 0.0, 1.0, 1.0f },
	{ "oct2Vol", "O2 Vol", 0.0, 1.0, 0.75f },
	{ "oct3Vol", "O3 Vol", 0.0, 1.0, 0.5f },
	
	{ "oct1Spread", "O1 Spread", 0.0, 1.0, 0.2f },
	{ "oct2Spread", "O2 Spread", 0.0, 1.0, 0.3f },
	{ "oct3Spread", "O3 Spread", 0.0, 1.0, 0.4f },
	
	{ "masterTune", "Master Tune", 400.0, 500.0, 440.0 },
	{ "pitchAmount", "Pitch Amount", 0.0, 1.0, 0.333f }, // 1x mult to 4x mult
	{ "pitchTime", "Pitch Time", 0.0, 1.0, 0.3f },
	
	{ "cutoff", "Cutoff", 0.0, 1.0, 0.5 },
	{ "cutoffAmount", "Cutoff Amount", 0.0, 1.0, 0.5 },
	{ "cutoffTime", "Cutoff Time", 0.0, 1.0, 0.5 },
	
	{ "release", "Release", 0.0, 1.0, 0.05 },
	{ "organ", "Organ", 0.0, 1.0, 0.0 },
	{ "grit", "Grit", 0.0, 1.0, 0.0 }
};
/**
O1 Vol			O2 Vol				O3 Vol
O1 Spread		O2 Spread			O3 Spread
Master Tune		Pitch Amount		Pitch Time
Cutoff			Cutoff Amount		Cutoff Time
Release			Organ				Grit
*/