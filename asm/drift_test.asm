LD V3, 0

LD V4, 0

LD V5, 240
LD DT, V5

LD V7, 13
LD V8, 18
LD V9, 24
LD VA, 30

JP draw

loop:
	LD V6, DT
	SNE V6, V3
	JP increment
	JP loop

increment:
	ADD V4, 1
	LD DT, V5
	JP draw

draw:
	LD I, 768
	LD B, V4
	LD V2, [I]

	CLS
	
	LD F, V0
	DRW V8, V7, 5
	LD F, V1
	DRW V9, V7, 5
	LD F, V2
	DRW VA, V7, 5

	JP loop
