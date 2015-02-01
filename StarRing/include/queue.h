/*
 * queue.h
 *
 *  Created on: 2014/05/09
 *      Author: StarRing
 */

/*
 * 単純なキューの実装
 */
#pragma once




//リングバッファキュー
typedef struct queue {

	unsigned int next;
	unsigned char *data;
	unsigned int size;
} queue_t;


void init_queue(queue_t *queue, unsigned int size);

void enqueue(queue_t *queue, unsigned char data);

unsigned char dequeue(queue_t *queue);



