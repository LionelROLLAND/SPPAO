#!/usr/bin/env python

import pygame

filename = "data/testDijkstra3615.txt"

lines = []
with open(filename, "r") as fd:
	lines = fd.readlines()


BLACK = (0, 0, 0)
GRUE = (0, 255, 255)
RED = (255, 0, 0)

def get_graph(lines):
	i = 0
	line = lines[i].strip("\n")
	points = []
	while line != "":
		sep = line.split(" ")
		if len(sep) == 6:
			points.append((int(sep[0]), float(sep[1]), float(sep[2]), int(sep[3]), int(sep[4]), int(sep[5]) ))
		else:
			points.append((int(sep[0]), float(sep[1]), float(sep[2]) ))
		i+=1
		line = lines[i].strip("\n")



	no_max = max(points, key= lambda x: x[0])[0]
	tab_points = [[] for j in range(no_max)]

	for e in points:
		tab_points[e[0]-1] = e[1:]




	i+=1
	connections = []
	line = lines[i].strip("\n")
	while line != "" and i < len(lines):
		sep = line.split(" ")
		if len(sep) == 6:
			connections.append((int(sep[0]), int(sep[1]), float(sep[2]), int(sep[3]), int(sep[4]), int(sep[5]) ))
		else:
			connections.append((int(sep[0]), int(sep[1]), float(sep[2]) ))


		i+=1
		if i < len(lines):
			line = lines[i].strip("\n")
	return tab_points, connections

def display_graph(tab_points, connections):
	#print(tab_points)
	#print(connections)
	#print(len(tab_points))
	Xmax = max(tab_points, key = lambda x: x[0])[0]
	Xmin = min(tab_points, key = lambda x: x[0])[0]
	Ymax = max(tab_points, key = lambda x: x[1])[1]
	Ymin = min(tab_points, key = lambda x: x[1])[1]

	width = 1800
	height = 980
	aX = width/(Xmax-Xmin)
	bX = -Xmin*aX

	aY = height/(Ymax-Ymin)
	bY = -Ymin*aY


	size = (width, height)

	pygame.init()
	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("Random planar graph")

	screen.fill(BLACK)

	for seg in connections:
		tab = tab_points[seg[0]-1]
		pos1 = (aX*tab[0]+bX, aY*tab[1]+bY)
		tab = tab_points[seg[1]-1]
		pos2 = (aX*tab[0]+bX, aY*tab[1]+bY)
		if len(seg) == 6:
			col = (seg[3], seg[4], seg[5])
		else:
			col = RED
		pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=4)

	for point in tab_points:
		pos = (aX*point[0]+bX, aY*point[1]+bY)
		if len(point) == 5:
			col = (point[2], point[3], point[4])
		else:
			col = GRUE
		pygame.draw.circle(screen, col, pos, 4)


	playing = True
	clock = pygame.time.Clock()
	
	while playing :
		clock.tick(30)
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				playing = False
		pygame.display.flip()

res = get_graph(lines)
display_graph(res[0], res[1])
