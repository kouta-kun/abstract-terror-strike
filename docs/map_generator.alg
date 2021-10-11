Map Building Algorithm
----
buildMap -> map<64>:
	 chests[8] = {}
	 map_tiles[64,64] = {{0}}
	 wallsAround(map_tiles)
	 doors[] = {}
	 recursively subdivide map, as:
	 	     