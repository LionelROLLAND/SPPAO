#!/usr/bin/env python

import pygame
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--no-fancy', dest='fancy', action='store_const',
                    const=False, default=True,
                    help='Fancy mode for the displaying of the graph')
parser.add_argument('--animated', dest='animated', action='store_const',
					const=True, default=False)
parser.add_argument("graph", type=str, help='Graphe a visualiser')
args = parser.parse_args()


filename = args.graph
#filename = "data/testSPPAO1.txt"
fancy = args.fancy
animated = args.animated

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


def getBetterGraph(lines):
	i = 0
	line = lines[i].strip("\n")
	points = []
	while line != "":
		sep = line.split(" ")
		points.append((int(sep[0]), float(sep[1]), float(sep[2]) ))
		i+=1
		line = lines[i].strip("\n")


	no_max = max(points, key= lambda x: x[0])[0]
	tab_points = [[points[0][1], points[0][2]] for j in range(no_max+1)]

	for e in points:
		tab_points[e[0]] = e[1:]

	plotings = []
	i+=1
	line = lines[i].strip("\n")
	while line != "" and i < len(lines):
		#print(sep)
		sep = line.split(" ")
		if sep[0] == "point":
			plotings.append(("point", int(sep[1]), int(sep[2]), float(sep[3]), int(sep[4]),
								int(sep[5]), int(sep[6]), int(sep[7]) ))
		if sep[0] == "arc":
			plotings.append(( "arc", int(sep[1]), int(sep[2]), float(sep[3]), int(sep[4]), 
								int(sep[5]), int(sep[7]), int(sep[8]), int(sep[9]) ))
		if sep[0] == "info":
			plotings.append(( "info", int(sep[1]), int(sep[2]), float(sep[3]), float(sep[4]) ))


		i+=1
		if i < len(lines):
			line = lines[i].strip("\n")


	plotings.sort(key=(lambda x: x[1] + x[2]))

	return tab_points, plotings


def display_graph(tab_points, connections):
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




def displayBetterGraph(tab_points, plotings):
	#print(tab_points)
	#print(connections)
	#print(len(tab_points))
	Xmax = max(tab_points, key = lambda x: x[0])[0]
	Xmin = min(tab_points, key = lambda x: x[0])[0]
	Ymax = max(tab_points, key = lambda x: x[1])[1]
	Ymin = min(tab_points, key = lambda x: x[1])[1]

	width = 1800
	height = 980
	a = min(width/(Xmax-Xmin), height/(Ymax-Ymin))
	bX = -a*Xmin

	bY = -a*Ymin


	size = ( int(a*(Xmax-Xmin)+1), int(a*(Ymax-Ymin)+1) )

	pygame.init()
	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("SPPAO1 result")

	varPlot = 0
	while varPlot < len(plotings) and plotings[varPlot][1] == 0:
		varPlot += 1

	#print("varPlot = ", varPlot)
	#print("len(plotings) = ", len(plotings))
	constPlot = 0

	executed = False
	while varPlot < len(plotings) or not(executed):
		executed = True


		screen.fill(BLACK)
		constPlot = 0
		if varPlot < len(plotings):
			currentLayer = plotings[varPlot][1]
		else:
			currentLayer = -1
		while (constPlot < len(plotings) and plotings[constPlot][1] == 0) \
					or (varPlot < len(plotings) and plotings[varPlot][1] == currentLayer):
			if constPlot < len(plotings):
				constDisp = plotings[constPlot]
			if varPlot < len(plotings):
				varDisp = plotings[varPlot]

	
			if varPlot >= len(plotings) or varDisp[1] != currentLayer:
				to_plot = constDisp
				constPlot += 1
			elif constPlot >= len(plotings) or constDisp[1] != 0:
				to_plot = varDisp
				varPlot += 1
			elif constDisp[2] <= varDisp[2]:
				to_plot = constDisp
				constPlot += 1
			else:
				to_plot = varDisp
				varPlot += 1

			if to_plot[0] == "point":
				pos = (a*tab_points[to_plot[4]][0]+bX, a*tab_points[to_plot[4]][1]+bY)
				col = (to_plot[5], to_plot[6], to_plot[7])
				scale = a*to_plot[3]
				pygame.draw.circle(screen, col, pos, max(scale,1))

			if to_plot[0] == "arc":
				pos1 = (a*tab_points[to_plot[4]][0]+bX, a*tab_points[to_plot[4]][1]+bY)
				pos2 = (a*tab_points[to_plot[5]][0]+bX, a*tab_points[to_plot[5]][1]+bY)
				col = (to_plot[6], to_plot[7], to_plot[8])
				scale = int(max(a*to_plot[3], 1))
				pygame.draw.line(screen, color=col, start_pos=pos1,
					end_pos=pos2, width=scale)

			if to_plot[0] == "info":
				pygame.display.set_caption("c = " + str(to_plot[3]) + ", d = " + str(to_plot[4]) )



		#pygame.image.save(screen, "data/ex_" + str(currentLayer) + ".png")
		playing = True
		clock = pygame.time.Clock()

		change_frame = pygame.event.custom_type()
		if animated:
			pygame.time.set_timer(change_frame, 200)
		
		while playing :
			clock.tick(30)
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					playing = False
					return
				elif event.type == pygame.MOUSEBUTTONUP:
					if event.button == 1:
						playing = False
				elif event.type == change_frame:
					playing = False
			pygame.display.flip()


if fancy:
	res = getBetterGraph(lines)
	displayBetterGraph(res[0], res[1])
else:
	res = get_graph(lines)
	display_graph(res[0], res[1])
