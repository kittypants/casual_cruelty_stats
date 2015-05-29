#include "UrT_XLR_Parser.hpp"

UrT_XLR_Parser::UrT_stats UrT_XLR_Parser::parse_XLR_stats()
{
	UrT_stats stats;
	std::string webpage_html;
	htmlDocPtr document_pointer;
	std::string map_name;
	std::vector<std::string> player_names;
	
	// prefixes commonly seen on map names, every prefix 
	// in this list is removed from a map name that might
	// have that prefix. This is to clean up the map names
	static std::vector<std::string> map_prefixes 
	{
		"ut4_"
		,"ut_"
	};

	// suffixes commonly seen on map names, every suffix
	// in this list is removed from a map name that might 
	// have that suffix. This is to clean up the map names
	static std::vector<std::string> map_suffixes 
	{
		"_bots"
		,"_fixed"
		,"_b10"
		,"_rc4"
		,"_4x"
		,"_b8"
	};
	
	// A list of all bot names that are seen on the server, 
	// all player names that start with these names are removed
	// to eliminate bots from out stats collection. I have 
	// never seen the start of a player name to match a bot name
	static std::vector<std::string> bot_names
	{
		"Boa"
		,"Cheetah"
		,"Chicken"
		,"Cobra"
		,"Cockroach"
		,"Cougar"
		,"Goose"
		,"Mantis"
		,"Penguin"
		,"Puma"
		,"Python"
		,"Raven"
		,"Scarab"
		,"Scorpion"
		,"Tiger"
		,"Guard"
		,"Commando"
		,"Soldier"
		,"Deathwish"
		,"Punisher"
		,"Lysander"
	};


	webpage_html		= get_webpage();
	if(failure) return get_failure();

	document_pointer	= get_document_pointer(webpage_html);
	if(failure) return get_failure();

	
	map_name			= get_map_name(document_pointer);
	if(failure) return get_failure();

	player_names		= get_player_names(document_pointer);
	if(failure) return get_failure();

	xmlFreeDoc(document_pointer);
	xmlCleanupParser();

	// remove map prefixes
	// TODO: figure out how to directly pass in the the required vector to the lambda function
	// instead of the catch all refrence
	std::for_each(map_prefixes.begin(), map_prefixes.end(), [&](std::string map_prefix)
	{
		if(map_name.find(map_prefix) == 0)
		{
			map_name = map_name.erase(0, map_prefix.size());
		}
	});

	// remove map suffixes
	std::for_each(map_suffixes.begin(), map_suffixes.end(), [&](std::string map_suffix)
	{
		if(map_name.size() > map_suffix.size())
		{
			if(map_name.compare(map_name.size() - map_suffix.size(), map_suffix.size(), map_suffix) == 0)
			{
				map_name = map_name.erase(map_name.size() - map_suffix.size(), map_name.size());
			}
		}
	});

	// remove bot names from the list of player names
	auto it = std::remove_if(player_names.begin(), player_names.end(), [&](std::string player_name)
	{
		return std::find_if(bot_names.begin(), bot_names.end(), [&player_name](std::string bot_name)
		{
			return player_name.find(bot_name) == 0;
		}) != bot_names.end();
	});
	player_names.erase(it, player_names.end());

	std::string kitty;
	for(auto player_name : player_names)
	{
		if(player_name.compare("KittyPants") == 0)
		{
			kitty = "true";		
		}
		else
		{
			kitty = "false";
		}
	}

	stats.player_count	= player_names.size();
	stats.player_names	= player_names;
	stats.map_name		= map_name;
	stats.snapshot_time = time(0);
	stats.kitty = kitty;

	return stats;
}


bool UrT_XLR_Parser::failure;
std::string UrT_XLR_Parser::errorBuffer;
const std::string UrT_XLR_Parser::error_message = "An error occured";
