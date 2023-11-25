/*
 * keyboard_tree.h
 *
 *  Created on: Sep 14, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_TREE_H_
#define KEYBOARD_TREE_H_

#include "lefl.h"
#include "keyboard.h"

#define KEYBOARD_TREE_ON

#ifdef KEYBOARD_TREE_ON
#define KEYBOARD_SLAVE_ON
#define KEYBOARD_MASTER_ON

#define SPI_SLAVE hspi3
#define SPI_MASTER hspi2
#endif

#define KEYBOARD_CHILD_NUM 4
#define KEYBOARD_CHILD_KEY_MAX_NUM 32
#define KEYBOARD_TREE_BUFFER_MAX_SIZE 256

typedef enum __Keyboard_TNode_Type
{
    KEYBOARD_TNODE_ROOT, KEYBOARD_TNODE_INTERNAL, KEYBOARD_TNODE_EXTERNAL,
} Keyboard_TNode_Type;

typedef enum __Keyboard_TNode_Status
{
    KEYBOARD_TNODE_DISCONNECTED,
    KEYBOARD_TNODE_CONNECTED,
    KEYBOARD_TNODE_DETECT,
    KEYBOARD_TNODE_READY,
    KEYBOARD_TNODE_ACTIVE,
} Keyboard_TNode_Status;

typedef struct __Keyboard_TNode
{
    Keyboard_TNode_Type Type;
    Keyboard_TNode_Status Status;
    lefl_bit_array_t Bitmap;
} Keyboard_TNode;

extern Keyboard_TNode_Type Keyboard_Tree_BaseType;
extern volatile Keyboard_TNode_Status Keyboard_Tree_BaseStatus;
extern lefl_bit_array_t Keyboard_Tree_ReportBitmap;

#ifdef KEYBOARD_MASTER_ON
extern uint8_t Master_SPI_TxBuffer[16];
extern uint8_t Master_SPI_RxBuffer[16];
void Keyboard_Tree_Detect();
void Keyboard_Tree_Scan();
#endif

#ifdef KEYBOARD_SLAVE_ON
extern Keyboard_TNode Keyboard_TNodes[KEYBOARD_CHILD_NUM];
extern uint8_t Slave_SPI_TxBuffer[16];
extern uint8_t Slave_SPI_RxBuffer[16];
void Keyboard_Tree_Report();
#endif

void Keyboard_Tree_Init();

#endif /* KEYBOARD_TREE_H_ */
