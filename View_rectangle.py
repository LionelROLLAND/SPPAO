#!/usr/bin/env python

import pygame
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("logs", type=str, help='Rectangles a visualiser')
args = parser.parse_args()

filename = args.logs

animated = False
BLACK = (0,0,0)
WHITE = (255, 255, 255)

lines = []
with open(filename, "r") as fd:
	lines = fd.readlines()


def getRect(lines):
	rects = []
	for line in lines:
		line = line.strip("\n")
		sep = line.split(" ")
		if sep[0] == "rect":
			rects.append(( sep[0], int(sep[1]), int(sep[2]), float(sep[3]), float(sep[4]),
				float(sep[5]), float(sep[6]), int(sep[7]), int(sep[8]), int(sep[9]) ))
		elif sep[0] == "point":
			rects.append(( sep[0], int(sep[1]), float(sep[2]), float(sep[3]), int(sep[4]),
				int(sep[5]), int(sep[6]) ))
	return rects


def displayRects(plotings):
	#print(tab_points)
	#print(connections)
	#print(len(tab_points))
	Xmax = 0
	Xmin = 0
	Ymax = 0
	Ymin = 0
	undefined = True
	for e in plotings:
		if e[0] == "rect":
			if undefined:
				Xmax = e[6]
				Xmin = e[5]
				Ymax = e[4]
				Ymin = e[3]
				undefined = False
			elif e[6] > Xmax:
				Xmax = e[6]
			elif e[5] < Xmin:
				Xmin = e[5]
			elif e[4] > Ymax:
				Ymax = e[4]
			elif e[3] < Ymin:
				Ymin = e[3]
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
		screen.fill(WHITE)
		nb_plots = 0
		varPlot = 0
		while varPlot < len(plotings):
			rect = plotings[varPlot]

			if rect[0] == "rect":

				rect = rect[1:]
				if step >= rect[0] and step < rect[1]:
					nb_plots += 1
					col = (rect[6], rect[7], rect[8])

					pos1 = (a*rect[4]+bX, a*(Ymax-rect[2] + Ymin)+bY)
					pos2 = (a*rect[4]+bX, a*(Ymax-rect[3] + Ymin)+bY)
					pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=7)

					pos1 = (a*rect[4]+bX, a*(Ymax-rect[3] + Ymin)+bY)
					pos2 = (a*rect[5]+bX, a*(Ymax-rect[3] + Ymin)+bY)
					pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=7)

					pos1 = (a*rect[5]+bX, a*(Ymax-rect[3] + Ymin)+bY)
					pos2 = (a*rect[5]+bX, a*(Ymax-rect[2] + Ymin)+bY)
					pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=7)

					pos1 = (a*rect[5]+bX, a*(Ymax-rect[2] + Ymin)+bY)
					pos2 = (a*rect[4]+bX, a*(Ymax-rect[2] + Ymin)+bY)
					pygame.draw.line(screen, color=col, start_pos=pos1, end_pos=pos2, width=7)
			
			elif rect[0] == "point":
				rect = rect[1:]
				if step >= rect[0]:
					pos = (a*rect[1]+bX, a*(Ymax-rect[2] + Ymin)+bY)
					col = (rect[3], rect[4], rect[5])
					pygame.draw.circle(screen, col, pos, 15)


			varPlot += 1


		#pygame.image.save(screen, "data/bothEx2_rec_" + str(step) + ".png")
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