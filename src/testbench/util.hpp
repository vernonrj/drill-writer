/* FILE:	util.h
 * AUTH:	Vernon Jones
 * DATE:	14 Mar 2012
 * VERSION:	0.2.4.2
 * DESC:	Library of portable functions currently used
 * 		for my CS163 class
 *
 */

// Preprocessor directives
// Default string size, only needed when size must be guessed.
// Try very hard not to guess.
#define DEFSIZE 128
// Use color for STDIO prompts (use/don't use 1/0)
// WARNING: Terminal is not tested for color! Highly experimental
// compile with USE_COLOR = 1 to enable
#define USE_COLOR 1

// Includes
#include <iostream>	// Standard I/O
#include <fstream>	// File I/O
#include <stdarg.h> 	// Variable-length arguments
#include <cstdlib>	// atoi()
#include <math.h>	// log(), pow()


// Namespace
using namespace std;



// Contains:
// Queue Class members
// Character Attributes Functions
// Character Storage Functions
// Character Formatting Functions
// Time Functions
// Standard I/O Functions
// File I/O Functions



// Queue Class members
struct qu_list
{
	// fifo node for queue linear linked list
	char *data;
	qu_list *next;
	// Used to specify an operator
	int iop;
};



class queue
{
	// Abstract data type for a message fifo
	public:
		// Constructor
		queue(void);
		// Check to see if data is present
		bool has_data(void);
		// Number of entries
		int qlen(void);
		// Size of oldest entry
		int elen(void);
		// Size of largest entry
		int max(void);
		// Make a new container
		int enqueue(const char*);
		// Move data from queue to queue
		void mvTo(queue*);
		void mvFrom(queue*);
		// Copy data from queue to queue
		void cpTo(queue*);
		void cpFrom(queue*);
		// Move around without dequeuing
		int rotate(char*&);
		int rotate(void);
		void rotate_reset(void);
		// Move queue insertion point
		void rotHead(char*);
		void syncRot(void);
		// Get operator of rot node
		int getrotop(void);
		// Set/Get operator value of newest node
		void setop(int);
		int getop(void);
		// Destroy
		int dequeue(char*&);
		int dequeue(void);
		// Empty all data from queue
		void discard(void);
		// Destructor
		~queue(void);
	private:
		// Structure
		qu_list *head;
		qu_list *rot;
};



// General Utility
// Character Attributes Functions
// (deprecated): wrapper for arrlen
int len (const char *str);		
// return length of charray
int arrlen (const char*);	
// Check if String is num
int chknumvalid(char *num);		
// Check Single Char
int chknumvalid(char num);		
// return 1 if str1 higher in the alphabet
int arrless(char *str1, char *str2);	
// equality check
int arreq(const char *str1, const char *str2); 
// check existance of arg2 in arg1
bool arrhas(const char*, const char*);	
// show instance of arg2 in arg1
int arrwhere(const char*, const char*);	

// Character Storage Functions

// store arg3 bytes of str2 to str1
void arrstore(char *str1, const char *str2, int stlen); 
// store arg2 into arg1
void arrstore(char *str1, const char *str2);	
// cat arg2 into arg1
void arrappend(char*&, const char*);
// convert integer in num to character in ch
void inttochar(int num, char *ch);
// formatted conversion of const char*--> into arg1 of size arg2
int arstrf(char*, int, const char*, ...);

// Character Formatting Functions

// Convert to lower case
void caseconvert(char *myinput);
// Keep arg1 before pivot point c
int keepxcut(char *myin, char c);
// Keep arg1 after pivot point c
int cutxkeep(char *myin, char c);

// Time Functions
int formatCtime(char *date);



// Standard I/O Functions
// dialogs
// Change STOUT to specified color
int ch_color(const char*);
// display contents of queue, prompt user, return answer in queue
int fdialog(queue *fifo);
// error functions
void qError(char errinfo[]);
void qError(void);
// Take column1 entries in arg1, optional context in arg2, 
// return user choice
int menu(queue *fifo, queue *context);
int menu(queue *fifo);



// File I/O Functions
int utilLoad(queue*, char *fname);
int utilSave(queue*, queue*, char *fname);
int stripLine(char *ops, char *&targ);
bool grabline (char *&str, ifstream&);



