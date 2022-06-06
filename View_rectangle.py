#!/usr/bin/env python

import pygame

filename = "data/SPPAO2.log"

animated = False
BLACK = (0,0,0)

lines = []
with open(filename, "r") as fd:
	lines = fd.readlines()


def getRect(lines):
	rects = []
	for line in lines:
		line = line.strip("\n")
		sep = line.split(" ")
		rects.append((int(sep[0]), int(sep[1]), float(sep[2]), float(sep[3]), float(sep[4]),
			float(sep[5]), int(sep[6]), int(sep[7]), int(sep[8]) ))
	return rects


def displayRects(plotings):
	#print(tab_points)
	#print(connections)
	#print(len(tab_points))
	Xmax = max(plotings, key = lambda x: x[5])[5]
	Xmin = min(plotings, key = lambda x: x[4])[4]
	Ymax = max(plotings, key = lambda x: x[3])[3]
	Ymin = min(plotings, key = lambda x: x[2])[2]

	width = 1800
	height = 980
	a = min(width/(Xmax-Xmin), height/(Ymax-Ymin))
	bX = -a*Xmin

	bY = -a*Ymin


	size = ( int(a*(Xmax-Xmin)+1), int(a*(Ymax-Ymin)+1) )

	pygame.init()
	screen = pygame.display.set_mode(size)

	pygame.display.set_caption("SPPAO2 rectangles")



	step = 0
	nb_plots = 1
	while nb_plots:
		screen.fill(BLACK)
		nb_plots = 0
		varPlot = 0
		while varPlot < len(plotings):
			rect = plotings[varPlot]

			if step >= rect[0] and step < rect[1]:
				nb_plots += 1
				col = (rect[6], rect[7], rect[8])

				pos1 = (a*rect[4]+bX, a*(Ymax-rect[2] + Ymin)+bY)
				pos2 = (a*rect[4]+bX, a*(Ymax-rect[3] + Ymin)+bY)
				pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=3)

				pos1 = (a*rect[4]+bX, a*(Ymax-rect[3] + Ymin)+bY)
				pos2 = (a*rect[5]+bX, a*(Ymax-rect[3] + Ymin)+bY)
				pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=3)

				pos1 = (a*rect[5]+bX, a*(Ymax-rect[3] + Ymin)+bY)
				pos2 = (a*rect[5]+bX, a*(Ymax-rect[2] + Ymin)+bY)
				pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=3)

				pos1 = (a*rect[5]+bX, a*(Ymax-rect[2] + Ymin)+bY)
				pos2 = (a*rect[4]+bX, a*(Ymax-rect[2] + Ymin)+bY)
				pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=3)

			varPlot += 1



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

		step += 1


displayRects(getRect(lines))