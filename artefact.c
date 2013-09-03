int artefactPrecense( dbMainPlanetPtr planetd )
{
  if( !( planetd->special[2] ) )
    return -1;
  return ( planetd->special[2] - 1 );
}


/*
Ectroverse Artefacts round #1 - ...
*/

unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Observation Station", //ARTEFACT_1_BIT 1
"Altar of Storms", //ARTEFACT_2_BIT 2
"Ether Laboratory", //ARTEFACT_4_BIT 4
"Ether Palace", // ARTEFACT_8_BIT 8

"Tachyon Pulsar", //ARTEFACT_16_BIT 16
"Ether Solar Formula", //ARTEFACT_32_BIT 32
"Arrakis Spice Melange", //ARTEFACT_64_BIT 64
"Apotheosis of War", //ARTEFACT_128_BIT 128

"Ether Gardens", //ARTEFACT_ETHER_BIT 256
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Observation Station</b> grants everyone the Observe Planet operation!",
"The <b>Altar of Storms</b> increases by 25% the attack strength of all your military units when defending!!",
"The <b>Ether Laboratory</b> increases the Energy and Military research maximum by 25%!",
"The <b>Ether Palace</b> increases your energy production by 25%!",

"The <b>Tachyon Pulsar</b> doubles the strenght of Phantoms and increases your unit upkeep by 50%!",
"The <b>Ether Solar Formula</b>, increases the output of Solar Collectors by 30%!",
"The <b>Arrakis Spice Melange</b> increases the production of Mineral by 50%!",
"The <b>Apotheosis of War</b> reduces by half the readiness loss for the Energy Surge incantation!", 

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"artimg4.gif",
"artimg12.gif",
"ether3.gif",
"ether2.gif",

"artimg3.gif",
"ether4.gif",
"artimg2.gif",
"artimg14.gif",

"artimg0.gif",
};

/*
Round 31-32-33


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Nuker",
"Ticking Time Bomb", 
"Exclusivity mine",
"Moral obelisk", 

"Speed demon", 
"Demon worker",
"Cystal summoner",
"Elit strategist",

"Ether Gardens",
};


unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Nuker</b> gives everyone Nuke planet operation!",
"The <b>Ticking Time Bomb</b> add 10% to each producton, but will explode each year!",
"The <b>Exclusivity mine</b> increases mineral production by 20%, but decreases ectrolium and crystal production by 5%!",
"The <b>Moral obelisk</b> halves your units upkeep, but reduces you energy production by 12%!",

"The <b>Speed demon</b> increases speed by 50% and increases construction time by 20%!",
"The <b>Demon worker</b> decreases speed by 30%, but decreases construction time by 30%!",
"The <b>Cystal summoner</b> increases maximum culture research by 70%!",
"The <b>Elit strategist</b> add 20% to maximum military research and substract 20% to tech research!",

"The <b>Ether Gardens</b> increase your energy production by 10%!", 
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg10.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg11.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg1.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};*/
/*
Round 31-32-33


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Uranium Mine",
"Lunar Tesseract", 
"Anti-Transmogrofiet-Polar",
"Foohon Ancestry", 

"Mana Gate", 
"Book of Revelations",
"Stealth Algorithm",
"Cloning Facilities",

"Ether Gardens",
};


un=
{
"T and increases output by 10%!", 
"Tep reduction by 30%!", 
"Tic strenght by 20% while cut the difficulity of all agent ops by 30%!", 
"T10000 population per tick extra!",  
"Tals by half!",
"T research maximum by 60% and the soldier unit strength by 20%, but reduces the technology research maximum by 30%!", 
"T transport units defense !", 
"Trowth by 50%!",
"The <b>Ether Gardens</b> increase your energy production by 10%!", 
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg10.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg11.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg1.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};
*/

/*
Round 14


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"High-Level Radioisotopes", 
"Nether Link", 
"Neural Interface",
"Diplomatic Core", 

"Technology Temple", 
"Library of Alexandria",
"Obelisk of Caesar",
"Hanging Gardens of Babylon",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>High-Level Radioisotopes</b> causes your nuke planet operation to reduce building spaces to 3/4(75%)!",
"The <b>Graveyard Ghosts</b> boost ghost ship strength by 75%!",
"The <b>Neural Interface</b> halves the build time of soldiers!",
"The <b>Diplomatic Core</b> Reveals other families allies to the artifact holder!",

"The <b>Technology Temple</b> increases EVERY max research by another 25%!",
"The <b>Library of Alexandria</b> each research center now produces 2 more research points!",
"The <b>Obelisk of Caesar</b> Reduces FR needed for exploration/attacking by 15%!",
"The <b>Hanging Gardens of Babylon</b> increases Population upkeep reduction by 30%!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg1.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};

*/

/*
Round 10


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Polyethers Cottage", 
"Furan House", 
"Alkoxy Place",
"Anhydrides Caverns", 

"Anisole City", 
"Aminal Lake",
"Acetal Forest",
"Ester Field",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Polyethers Cottage</b> increase mineral production by 6%!",
"The <b>Furan House</b> ectrolium production go up by 5%!",
"The <b>Alkoxy Place</b> reduce portal upkeep by 33%!",
"The <b>Anhydrides Caverns</b> give 50% more max operation research!",

"The <b>Anisole City</b> make place for 5000 other person per city!",
"The <b>Aminal Lake</b> give soldier 10% attack and reduce droid defence by 5%!",
"The <b>Acetal Forest</b> make mineral harder to mine by 30% but double cystal production%!",
"The <b>Ester Field</b> increase egg grow rate by 5%(Population)!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg1.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};
*/


/*
Round 9 and Event


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Citizen coke", 
"Ra bless", 
"Crazy scientist", 
"Cruiser capitain", 

"Fast scientist", 
"Larger appartement",
"Rich scientist",
"Portal builder",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Citizen coke</b> give you 5% bonus in ressources production!",
"The <b>Ra bless</b> grant you a good recolt, +20% populations growth!",
"The <b>Crazy scientist</b> make all research center produce 5% more points!",
"The <b>Cruiser fan</b> decrease cruiser upkeep by 20%!",

"The <b>Fast scientist</b> make possible that your research go up or down of 2% each tick!",
"The <b>Larger appartement</b> give space for 10000 more person a city!",
"The <b>Rich scientist</b> autodestruct after 90 ticks and doesn't deacrese your research funding!",
"The <b>Portal builder</b> make portal build twice faster!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};
*/


/*
Round 8


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Warp Node", 
"Democrats Battleplan", 
"Cloning Facilities", 
"Kamikazes", 

"Space Circus", 
"Enhanced Crystal",
"Einstein's Manual",
"Wormhole Conjunction",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Warp Node</b> halves portal upkeep and increases maximum Portal Research to 500%!",
"The <b>Democrats Battleplan</b> increases ectrolium production by 20% when not at war!",
"The <b>Cloning Facilities</b> increase maximum population by 10% and increase population growth by 20%!",
"The <b>Kamikazes</b> increase fighter attack strength by 30% but reduce 10% to their defence!",

"The <b>Space Circus</b> no stats increase, but your empire lights up pink in the rankings!",
"The <b>Enhanced Crystal</b> reduce the crystal decay by half and produce -10% crystal income!",
"The <b>Einstein's Manual</b> gives +1% max research for every field with every 5% of tech research you have!",
"The <b>Wormhole Conjunction</b> triples the speed of your ghost ships!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg9.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};
*/
/*
Round 7


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"United Empires", 
"Foohon Ancestry", 
"Enigma Machine", 
"Militia Academy", 

"Event Horizon", 
"Plasma Cage",
"Book of Senses",
"Robotic Sensors",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>United Empires</b> shows when players in any empire are online!",
"The <b>Foohon Ancestry</b> gives all races 1 RP per 10000 population per tick extra!",
"The <b>Enigma Machine</b> gives +10% agent strength, and makes you immune to spy target, infiltration, and high infiltration!",
"The <b>Militia Academy</b> provides satellite defense for planets based on population!",

"The <b>Event Horizon</b> gives +20% energy production, but -10% mineral, crystal and ectrolium production!",
"The <b>Plasma Cage</b> gives +50% max tech research and +20% faster research!",
"The <b>Book of Senses</b> tells the range for a sense artefact!",
"The <b>Robotic Sensors</b> reduces defense satellite upkeep by 90%!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg9.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};
*/

/*
Round 6


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Death Star", 
"Delayed Artefact", 
"Wasteland", 
"China's Production Manual", 

"Sentient Planet", 
"Stellar Construction symbiosis",
"Stellar Production Symbiosis",
"Bulk Transmitter",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Death Star</b> doubles portal upkeep and unit strength when defending, but limits portal coverage to 50%!",
"The <b>Delayed Artefact</b> if you keep this artefact for 144 ticks it will double all your other artefacts powers!",
"The <b>Wasteland</b> each 36 tick all your planets grow in size by 5!",
"The <b>China's Production Manual</b> reduces unit costs by 30% but halves their defence!",

"The <b>Deep Space Radar</b> eliminates enemy dark web effects!",
"The <b>Jewish Craftsman Guide</b> increases crystal production by 50%, but lowers mineral and ectrolium production by 20%!",
"The <b>Cloak Of Invisibility</b> hide your identity when performing ops!",
"The <b>Reversed Mysthical Cycle</b> converts all lost ghost ships into psychics!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg9.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};

*/

/*
Round 13


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Wilbo Battle Manual", 
"Universal Knowledge Dome", 
"Stellar Life Symbiosis", 
"Runestone", 

"Sentient Planet", 
"Stellar Construction symbiosis",
"Stellar Production Symbiosis",
"Bulk Transmitter",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Wilbo Battle Manual</b> increases the fleet readiness maximum to 125%!",
"The <b>Universal Knowledge Dome</b> increases the maximum of all research fields by 40%!",
"The <b>Stellar Life Symbiosis</b> reduces all unit upkeep costs by 10%!",
"The <b>Runestone</b> increases Crystal production by 20%!",

"The <b>Sentient Planet</b>  increase mineral, crystal and ectrolium production by 5%!",
"The <b>Stellar Construction Symbiosis</b> reduces all building upkeep costs by 10%!",
"The <b>Stellar Production Symbiosis</b> reduces all ressource decay by half!",
"The <b>Bulk Transmitter</b> increases the Ectrolium production rate by 30%!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg9.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
};
*/

/*
Round 12


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Cancellation Master", 
"Cybernetic Society", 
"Xxit's Wisdom Grimoire", 
"Temple of Enlightenment", 

"Warfare Nexus", 
"Tachyon Pulsar",
"Bulk Transmitter",
"Statis Field",

"Ether Gardens",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Cancellation Master</b> give you the power to ask full refund when cancelling!",
"The <b>Cybernetic Society</b> doubles research production, but increases the vulnerability to enemy agents operation by 50%!",
"The <b>Xxit's Wisdom Grimoire</b> increases by 15% the Fission reactors energy production!",
"The <b>Temple of Enlightenment</b> increases the Culture and Construction research maximums by 60%!",

"The <b>Warfare Nexus</b> increases both the attack and defence strength of all military units by 15%!",
"The <b>Tachyon Pulsar</b> doubles the effectiveness of Shield networks and increases research production by 20%!",
"The <b>Bulk Transmitter</b> increases the Ectrolium production rate by 30%!",
"The <b>Statis Field</b> reduces fleet readiness losses by 20% when attacking!",

"The <b>Ether Gardens</b> increase your energy production by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg10.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg9.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
};
*/

/*
Round 11 


unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Granary", 
"Fawcett's guide", 
"Subspace Rift", 
"Teleportation Router", 

"Soulfly", 
"Effisiont working guide",
"Purification Subprocessor",
"Box of the Beast",

"Wisdom of God",
"Terrorists handbook",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Granary</b> halves energy and crystal decay!",
"The <b>Fawcett's guide</b> decreases Fleet readiness costs by 20%!",
"A <b>Subspace Rift</b> reduces the speed of everyone in the galaxy by 10% except you!",
"The <b>Teleportation Router</b> eliminates portal upkeep and reduces portal construction time by 30%!",

"The <b>Soulfly</b> halves the cost of Ghost ship operations!",
"The <b>Effisiont working guide</b> removes fissions nw and each fission gives 1 Research point!",
"The <b>Purification Subprocessor</b> doubles planet's bonusses!",
"The <b>Box of the Beast</b> increases the attack strenght of your forces by 30%!",

"The <b>Wisdom of God</b> increases culture research speed by 20% and the psychics defence by 30%!",
"The <b>Terrorists handbook</b> halved psychic cost, with +50% maximum operations research!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg1.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg3.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg7.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg9.gif",
};

*/

/*
Round 10 (the server after ameede)
*/
/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens", 
"Nintendo Tetris Manual", 
"Enchanted Mana Stone", 
"Matrix Glasses Storage", 

"Golden Gate", 
"Polonium Recipe",
"Uranium Mine",
"Tesla's Lost Secrets",

"The Data Processing Sphere",
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> increase your energy production by 10%!",
"The <b>Nintendo Tetris Manual</b> lowers all buildings upkeep by 10%!",
"A <b>Enchanted Mana Stone</b> halves your psychics readiness loss but increases their cost by 50%!",
"The <b>Matrix Glasses Storage</b> reduces your agent losses by 10%!",

"The <b>Golden Gate</b> increases portal research maximum to 500 but triples their upkeep!",
"The <b>Polonium Recipe</b> makes enemy troops defend 20% less but increases fission upkeep 20%!",
"The <b>Uranium Mine</b> lowers fission upkeep by 15% and increases output by 20%!",
"The <b>Tesla's Lost Secrets</b> increase all research maximums by 50%!",

"The <b>Data Processing Sphere</b> increases the technology research production rate by 400%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://www.bluedolfie.com/ectroguide/images/artimg0.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg4.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg5.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg2.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg10.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg8.gif",
"http://www.bluedolfie.com/ectroguide/images/artimg11.gif",

"http://www.bluedolfie.com/ectroguide/images/artimg6.gif",
};
*/


/*
Round 9
*/

/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Wilbo Battle Manual",
"Horn of Wrath",
"Monolith of Empathy",

"Mind Worms Colony",
"Crystal Forest",
"Templar Archives",
"Data Processing Sphere",

"Foresight Theorem"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> provide a 10% increased Energy production!",
"The <b>Wilbo Battle Manual</b> increases the fleet readiness maximum to 125%!",
"The <b>Horn of Wrath</b> increases by 25% the attack strength of all military units when defending!",
"The <b>Monolith of Empathy</b> increases the research maximum of all fields by 40%, but reduces the attack strength of both sides by 30% in battle!",

"The <b>Mind Worms Colony</b> increases the psychics strength by 30%!",
"The <b>Crystal Forest</b> increases the crystal production by 10%, and removes the decay rate!",
"The <b>Templar Archives</b> increases the maximum of all reasearch fields by 30%!",
"The <b>Data Processing Sphere</b> increases the technology research production rate by 400%!",

"The <b>Foresight Theorem</b> increases agents strength by 30% and the defence strength of all military units by 15%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg11.gif",

"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
};
*/





/*
Round 8
*/

/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Cybernetic Society",
"Xxit's Wisdom Grimoire",
"Temple of Enlightenment",

"Mana Tree",
"Star of Life",
"Obelisk of Time",
"Hyperspace Theorem",

"Invisibility Algorithm"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
  "The <b>Ether Gardens</b> provide a 10% increased Energy production!",
  "The <b>Cybernetic Society</b> doubles research production, but increases the vulnerability to enemy agents operation by 50%!",
  "The <b>Xxit's Wisdom Grimoire</b> increases by 15% the Fission reactors energy production!",
  "The <b>Temple of Enlightenment</b> increases the Culture and Construction research maximums by 60%!",

  "The <b>Mana Tree</b> increases the psychics strength by 30%!",
  "The <b>Star of Life</b> increases the research maximum of all fields by 80%, but reduces the attack strength of all military units by 25%!",
  "The <b>Obelisk of Time</b> increases all readiness recovery rates by 15%!",
  "The <b>Hyperspace Theorem</b> increases the construction research maximum by 100%!",

  "The <b>Invisibility algorithm</b> increases the attack strength of all military units by 20%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg11.gif",

"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
};
*/



/*
Round 7
*/

/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Warp Fields Paradigm",
"Afterlife Gate",
"Operation Center",

"Warfare Nexus",
"Tachyon Pulsar",
"Universal Knowledge Dome",
"Peace Sanctuary",

"Invisibility Algorithm"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
 "The <b>Ether Gardens</b> provide a 10% increased Energy production!",
 "The <b>Warp Fields Paradigm</b> increases the Portal research maximum by 120%!",
 "The <b>Afterlife Gate</b> doubles the Phantoms spell effectiveness!",
 "The <b>Operation Center</b> increases agents strength by 20% in offense and 50% in defense!",

 "The <b>Warfare Nexus</b> increases both the attack and defense strength of all military units by 15%!",
 "The <b>Tachyon Pulsar</b> doubles the effectiveness of Shield networks and increases research production by 20%!",
 "The <b>Universal Knowledge Dome</b> increases the maximum of all research fields by 40%!",
 "The <b>Peace Sanctuary</b> increases the defense strength of all military units by 40% when defending!",

 "The <b>Invisibility algorithm</b> increases the attack strength of all military units by 30%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg11.gif",

"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
};
*/





/*
Round 6
*/

/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Light of Judgement",
"Bulk Transmitter",
"Stasis Field",

"Crystal Temple",
"Universal Backnode",
"Quantum Singularity",
"Subspace Pulsar",

"Transcendence Pathway"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> provide a 10% increased Energy production!",
"The <b>Light of Judgement</b> increases the attack strength of all military units by 30%!",
"The <b>Bulk Transmitter</b> increases the Ectrolium production rate by 30%!",
"The <b>Statis Field</b> reduces fleet readiness losses by 20% when attacking!",

"The <b>Crystal Temple</b> produces 3 research points for each Crystal Laboratory!",
"The <b>Universal Backnode</b> increases research production by 30%!",
"The <b>Quantum Singularity</b> increase by 60% the Energy production research maximum!",
"The <b>Subspace Pulsar</b> increases the agents effectiveness by 35%!",

"The <b>Transcendence Pathway</b> increase all military units defence by 20%, psychics strength by 30%, and the maximum of all research fields by 40%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg11.gif",

"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
};

*/








/*
Round 5
*/

/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Alan Schezar Battle Bible",
"Fanatics Tower",
"Tome of Infinite Knowledge",

"Living Planet",
"Space folding prism",
"Sanctuary of Light",
"Sanctuary of Darkness",

"Universal Translator"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> provide a 10% increased Energy production!",
"The <b>Alan Schezar Battle Bible</b> increases by 50% both the attack and defense strength of goliath units!",
"The <b>Fanatics Tower</b> increases the empire agents effectiveness by 30%!",
"The <b>Tome of Infinite Knowledge</b> increase the maximum of all research fields by 30%!",

"The <b>Living Planet</b> increases Mineral and Ectrolium production by 10%!",
"The <b>Space folding prism</b> increase the Portal research maximum by 120%!",
"The <b>Sanctuary of Light</b> increases by 30% the defense strength of all military units, only when defending!",
"The <b>Sanctuary of Darkness</b> increases by 30% the attack strength of all military units, only when attacking!",

"The <b>Universal Translator</b> increases the Culture research maximum by 100%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg11.gif",

"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
};
*/









/*
Round 4
*/

/*
unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Arrakis Spice Melange",
"Espers Gate",
"Afterlife Portal",

"Antimatter Geyser",
"Sub-quark Society",
"Apotheosis of War",
"Tree of Life"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> provide a 10% increased Energy production!",
"The <b>Arrakis Spice Melange</b> reduces by half the Portals upkeep costs, and increase Soldiers strength by 30%!",
"The <b>Espers Gate</b> increases Psychics and Ghost Ships strength by 30%!",
"The <b>Afterlife Portal</b> raises Operations and Technology research maximum by 60%!",

"The <b>Antimatter Geyser</b> reduces by half the difficulty and readiness loss of the Nuke Planet operation!",
"The <b>Sub-quark Society</b> raises Construction research maximum by 80%!",
"The <b>Apotheosis of War</b> increases all military units attack by 30% while reducing their defence by 10%!",
"The <b>Tree of Life</b> doubles population growth, and raises Culture research maximum by 40%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
};
*/










/*
Beta 5

Ether Gardens  +5% energy production
Mana Tree  +30% psychics spells effectiveness
Evolution Obelisk  +20% raise maximum for all research fields
Horn of Czarina  +10% military units defense
Aura Pyramid  -20% fleet readiness losses
Altar of Storms  +20% military units attack, -10% military units defense

unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Mana Tree",
"Evolution Obelisk",
"Horn of Czarina",
"Aura Pyramid",
"Altar of Storms"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"<b>Ether Gardens</b> provide a 5% increased energy production!",
"The <b>Mana Tree</b> increases the effectiveness of your psychics by 30%!",
"The <b>Evolution Obelisk</b> raises the maximum of all research fields by 20%!",
"The <b>Horn of Czarina</b> provides a bonus of 10% to all military units attack!",
"The <b>Aura Pyramid</b> reduces fleet readiness losses when attacking by 20%!",
"The <b>Altar of Storms</b> increases military units strength by 20% while reducing their defense by 10%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg2.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg4.gif",
"http://users.pandora.be/amedee/ectroverse/artimg5.gif"
};
*/




/*
Round 1

Ether Gardens  +10% energy production
Espers Gate   Increase Psychics strength by +20% and the effectiveness of Phantoms spell by 50% 
Foutain of Jouvence   Increase population growth by 50% and Culture research maximum by 30%
Antimatter Collider   Increase fission reactors effectiveness by 10% and raise all research fields maximum by 10%
Regeneration Dome  Crystal decay rate reduced to 25% of the regular rate
Sphere of Fury   Increase military units attack by +25%, reduce agents and psychics strength by 15%
Minbari Legacy   Increase military units defense by 15%, fleets travel speed increased by 30%
Empath Nexus   Reduces by half the difficulty of the operations : Spy Target, Observe Planet, Infiltration and High Infiltration

unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Espers Gate",
"Fountain of Jouvence",
"Antimatter Collider",

"Regeneration Dome",
"Sphere of Fury",
"Minbari Legacy",
"Empath Nexus"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> provide a 10% increased Energy production!",
"The <b>Espers Gate</b> increases Psychics strength by 20% and the effectiveness of the Phantoms spell by 50%!",
"The <b>Fountain of Jouvence</b> increases population growth rate by 50% and raise Culture research maximum by 30%!",
"The <b>Antimatter Collider</b> increases Fission reactors production by 10% and raise all research fields maximum by 10%!",

"The <b>Regeneration Dome</b> reduces the Crystal decay rate to 25% of the regular rate!",
"The <b>Sphere of Fury</b> increases military units attack by 25%, while reducing agents and psychics strength by 15%!",
"The <b>Minbari Legacy</b> increases military units defense by 15% and fleets travel speed by 30%!",
"The <b>Empath Nexus</b> reduces by half the difficulty of the operations : Spy Target, Observe Planet, Infiltration and High Infiltration!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg6.gif",

"http://users.pandora.be/amedee/ectroverse/artimg10.gif",
"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
};
*/





/*
Round 2

Ether Gardens  increase energy production by 10%
Travel Hypercube  increase speed by 30% and maximum portal research by 60%
Vulcan Library  raise all research maximums by 15%, or 40% with the Vulcan Forge
Vulcan Forge  increase all military units attack and defense by 5%, or 15% with the Vulcan Library

Stealth Dome  increase all military units defense by 10% and agents effectiveness by 30%
Creation Temple  all planets owned by the empire increase their size by 1 per 12 weeks
Energy Paradigm  increase energy production by 20% and double the decay rate
Sentient Planet  increase mineral, crystal and ectrolium production by 5%

unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Travel Hypercube",
"Vulcan Library",
"Vulcan Forge",

"Stealth Dome",
"Creation Temple",
"Energy Paradigm",
"Sentient Planet"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> increase energy production by 10%!",
"The <b>Travel Hypercube</b> increases speed by 30% and maximum portal research by 60%!",
"The <b>Vulcan Library</b> raises all research maximums by 15%, or 40% with the Vulcan Forge!",
"The <b>Vulcan Forge</b> increases all military units attack and defense by 5%, or 15% with the Vulcan Library!",

"The <b>Stealth Dome</b> increases all military units defense by 10% and agents effectiveness by 30%!",
"The <b>Creation Temple</b> increases the size of all planets owned by the empire by 1 per 12 weeks!",
"The <b>Energy Paradigm</b> increases energy production by 20% and double the decay rate!",
"The <b>Sentient Planet</b> increases mineral, crystal and ectrolium production by 5!%"
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg6.gif",

"http://users.pandora.be/amedee/ectroverse/artimg10.gif",
"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
};
*/




/*
Round 3

unsigned char *artefactName[ARTEFACT_NUMUSED] =
{
"Ether Gardens",
"Universal Knowledge Archives",
"Stellar Life Symbiosis",
"Runestone",

"Tachyon Pulsar",
"Time Field Paradigm",
"Global Consciousness Nexus",
"Vorlon Weaponry Relics"
};

unsigned char *artefactDescription[ARTEFACT_NUMUSED] =
{
"The <b>Ether Gardens</b> provide a 10% increased Energy production!",
"The <b>Universal Knowledge Archives</b> doubles technology research production and raises all research maximums by 40%!",
"The <b>Stellar Life Symbiosis</b> reduces all upkeep costs by 10%!",
"The <b>Runestone</b> increases Crystal production by 20%!",

"The <b>Tachyon Pulsar</b> doubles Shield Networks strength!",
"The <b>Time Field Paradigm</b> reduces the readiness cost of all agents operations by half!",
"The <b>Global Consciousness Nexus</b> raises the Culture research maximum by 150%!",
"The <b>Vorlon Weaponry Relics</b> increase all military units attack by 20%!",
};

unsigned char *artefactImage[ARTEFACT_NUMUSED] =
{
"http://users.pandora.be/amedee/ectroverse/artimg0.gif",
"http://users.pandora.be/amedee/ectroverse/artimg6.gif",
"http://users.pandora.be/amedee/ectroverse/artimg8.gif",
"http://users.pandora.be/amedee/ectroverse/artimg1.gif",

"http://users.pandora.be/amedee/ectroverse/artimg9.gif",
"http://users.pandora.be/amedee/ectroverse/artimg3.gif",
"http://users.pandora.be/amedee/ectroverse/artimg10.gif",
"http://users.pandora.be/amedee/ectroverse/artimg7.gif",
};
*/



