/* NAME:	Vernon Jones
 * DATE:	14 Mar 2012
 * CLASS:	CS163
 * FILE:	util.cpp
 * VERSION:	0.2.4.2
 *
 * For more information, please read util.h header file
 */

#include "util.hpp"

// Contains:
// Queue Class members
// Character Attributes Functions
// Character Storage Functions
// Character Formatting Functions
// Time Functions
// Standard I/O Functions
// File I/O Functions


// Queue Class members



queue::queue(void)
{
	// Constructor for queue class
	head = 0;
	rot = 0;
}



queue::~queue(void)
{
	// Destructor for queue class
	// Dump any data left over
	// Dump any data left over
	while (has_data())
		dequeue();
}



bool queue::has_data(void)
{
	// Checks to see if there's data present in queue
	// INPUT:	internal
	// OUTPUT:	in return
	// returns:	data present/empty (1/0)
	
	// If there is a valid location at head, assume there is data to read.
	// the queue is set up to have head point to NULL when there is no data
	if (head) 
		return 1;
	return 0;
}



int queue::qlen(void)
{
	// Get number of entries in queue
	// INPUT:	internal
	// OUTPUT:	in return
	// returns:	number of entries in queue
	
	// Count var
	int count = 1;
	// Loop list
	qu_list *last;

	// Get number of entries
	if (head == NULL)
	{
		// No entries
		return 0;
	}
	else if (head == head -> next)
	{
		// 1 entry
		return 1;
	}
	for (last=head->next; last != head; last = last->next)
	{
		// count how many entries there are
		count++;
	}

	return count;
}



int queue::elen(void)
{
	// Check size of oldest data
	
	int size;
	qu_list *last;

	// Make sure there's data
	if (!head)
		return 0;
	// Go to oldest data
	last = head->next;
	// Get size of oldest data
	size = arrlen(last->data);

	return size;
}



int queue::max(void)
{
	// Get largest entry
	
	int size;
	qu_list *last;
	if (!head)
		return 0;
	size = arrlen(head->data);
	// Look for largest entry
	for (last = head->next; last != head; last = last->next)
	{
		if (size < arrlen(last->data))
			size = arrlen(last->data);
	}
	return size;
}



int queue::enqueue(const char *data)
{
	// Put data into the queue
	// INPUT:	{qdata}: new data for queue
	// OUTPUT:	none
	// returns:	Execution success/failure (0/-1)
	
	// Sizing for memory allocation
	int size;
	qu_list *newnode;
	qu_list *last;

	newnode = new qu_list;
	if (head == NULL)
	{
		// Making first node in circ. linked list
		head = newnode;
		head->next = head;
	}
	else
	{
		// Making new node in circ. linked list
		// Put last at the oldest data
		last = head -> next;
		// Use the new node as a new head
		head -> next = newnode;
		// Link the new head to the old head
		newnode -> next = last;
		// make newnode the new head
		head = newnode;
	}
	head->iop = 0;

	size = arrlen(data)+1;
	head->data = new char [size];
	arrstore(head->data, data);
	return 0;
}



// Move data from queue to queue
void queue::mvTo(queue* fifo)
{
	// Move data from here to fifo
	char *ctemp;

	ctemp = new char [1];
	// Move our data to foreign queue
	while (has_data())
	{
		// Take data
		dequeue(ctemp);
		// Put it in other queue
		fifo->enqueue(ctemp);
	}
	delete [] ctemp;
	return;
}



void queue::mvFrom(queue *fifo)
{
	// Move data from other queue here
	char *ctemp;

	ctemp = new char [1];
	// Move foreign queue data here
	while (fifo->has_data())
	{
		// Take data
		fifo->dequeue(ctemp);
		// Put it in our queue
		enqueue(ctemp);
	}
	delete [] ctemp;
	return;
}



// Copy data from queue to queue
void queue::cpTo(queue *fifo)
{
	// Copy data from here to foreign queue
	char *ctemp;

	ctemp = new char [1];
	// Move our data to foreign queue
	while (!rotate(ctemp))
	{
		// Put our data into other queue
		fifo->enqueue(ctemp);
	}
	delete [] ctemp;
	return;
}



void queue::cpFrom(queue *fifo)
{
	// Copy data foreign queue to here
	char *ctemp;

	ctemp = new char [1];
	// Move our data to foreign queue
	while (!fifo->rotate(ctemp))
	{
		// Put our data into other queue
		enqueue(ctemp);
	}
	delete [] ctemp;
	return;
}



int queue::rotate(char *&data)
{
	// Get a piece of data out of the queue, shift to the next spot
	int size;
	// Make sure there's data
	if (head == NULL)
	{
		return -1;
	}
	// If our rot pointer isn't defined,
	// move to oldest piece of data
	if (rot == NULL)
		rot = head->next;
	delete [] data;
	size = arrlen(rot->data)+1;
	// Reallocate data
	data = new char [size];
	arrstore(data, rot->data);
	// increment
	rot = rot->next;
	// Check to see if that was the last node
	if (rot == head->next)
	{
		// Last node, put rot at beginning again
		rot = 0;
		return 1;
	}
	// not last node
	return 0;
}



int queue::rotate(void)
{
	// Shift to the next spot
	if (head == NULL)
		return -1;
	// If our rot pointer isn't defined,
	// move to oldest piece of data
	if (rot == NULL)
		rot = head->next;
	rot = rot->next;
	if (rot == head->next)
	{
		// Last node, put rot at beginning again
		rot = 0;
		return 1;
	}
	// Not last node
	return 0;
}



void queue::rotate_reset(void)
{
	// Reset rotate pointer to oldest data
	if (head)
		rot = head->next;
	else
		rot = 0;
	return;
}



void queue::rotHead(char *ctemp)
{
	// Move the insertion point in the queue
	// return the "oldest data" in the queue
	qu_list *last;
	if (!head)
		return;
	// Move head
	head = head->next;
	// Move to new "old data"
	last = head->next;
	// Re-init temporary charray
	delete [] ctemp;
	ctemp = new char [arrlen(last->data)+1];
	// Store new "old data" to charray
	arrstore(ctemp, last->data);

	return;
}



void queue::syncRot(void)
{
	// Move the head so the next piece out of the queue
	// is what was just rotated out
	// so head->next = rot

	qu_list *last;
	qu_list *curr;
	// Make sure there are pointers to use
	if (!head)
		return;
	if (!rot)
		return;

	// Make sure we don't have to go through the entire list
	last = head->next;
	if (last->next == rot)
		return;
	curr = last->next;
	while (curr->next != rot)
	{
		// Find the point where the next dequeue will
		// give the rot pointer
		if (curr == head)
			break;
		if (!curr)
			break;
		last = curr;
		curr = curr->next;
	}
	// Make sure nothing went weird
	if (last)
	{
		// pointer is valid to use
		head = last;
	}
	return;
}



int queue::getrotop(void)
{
	// Get operation of rotated node
	if (!rot)
		return -1;
	return rot->iop;
}

void queue::setop(int num)
{
	// Set the operator on the newest queue.
	// Used to specify the type of data coming in / going out
	
	head->iop = num;
	return;
}



int queue::getop(void)
{
	// Get the operator of the oldest queue.
	int num;
	qu_list *last;

	// Check to make sure there's a node
	if (!head)
		return -1;
	// Go to oldest data
	last = head->next;
	// Get operation of oldest data
	num = last->iop;
	// Return operation on oldest data
	return num;
}



int queue::dequeue(char *& data)
{
	// Take data out of queue
	// INPUT:	none, taken internally
	// OUTPUT:	{data}: initialized, taken from oldest queue
	// returns:	operator/error (1/0/-1)
	
	// Newest data pointer
	qu_list *last;
	// Size of data
	int size;
	// operator
	int iop;

	// Make sure there's data to dequeue
	if (head == NULL)
	{
		return -1;
	}
	// Move to New data
	last = head -> next;

	// Deallocate so we can put appropriate sizes in
	delete [] data;
	size = arrlen(last->data)+1;
	// Reallocate data
	data = new char [size];
	arrstore(data, last->data);
	iop = last->iop;

	// Remove dequeued node
	head->next = last->next;
	// Make sure we delete cleanly
	if (last == head)
	{
		// Deleting last node
		head = 0;
	}

	// Deallocate correctly
	delete [] last->data;
	delete last;

	// return operator included
	return iop;
}



int queue::dequeue(void)
{
	// dequeues and dumps the data from the list
	// INPUT:	none
	// OUTPUT:	none
	// returns:	Execution success/failure (0/-1)
	
	// DESC:	There are certain points in the program where dequeuing
	// 		is wanted, but the data is not. This function does that

	// Dummy char
	char *dummy;
	// exit code
	int excode;

	// Allocate dummy data
	dummy = new char [1];
	// Dequeue to dummy char
	excode = dequeue(dummy);
	// Kill dummy char
	delete [] dummy;
	
	// Return success/failure
	return excode;
}



void queue::discard(void)
{
	// Dump all data from the queue
	while (has_data())
		dequeue();
	return;
}



/* Character Attributes Functions
 * These functions find attributes for either
 * one array, or many.
 * Operations like finding the size of the array,
 * checking equality, or alphabetical order 
 * go here
 */



int len (const char *str)
{
	// Wrapper for arrlen
	// Deprecated. replace len() with arrlen()

	// Length
	int mylen;

	// Get size
	mylen = arrlen(str);
	// Return size
	return mylen;
}



int arrlen (const char *str)
{
	// Get length of character string
	// INPUT:	{str}
	// OUTPUT:	size of {str} in return
	// returns:	size of {str}
	//
	// DESC:	Find length of string of characters given as "abcde"

	// length
	int mylen;
	// loop var
	int i;
	//int overf = esize + 1;

	// Get size
	for (i=0; str[i] != '\0'; i++)
	{
		// loop until we hit the end of the string
		// or get to a size that shouldn't happen
	}
	mylen = i;
	

	return mylen;
}



int chknumvalid(char *num)
{
	// Check to see if char contains valid num
	// INPUT:	{num}: a possible num stored in a char
	// OUTPUT:	in return
	// returns:	Negative/Valid/Invalid (2/1/0)
	
	// number index
	const char *index;
	// loop var
	int i, j;
	// Validity
	int isvalid;
	bool isneg;
	// Build index
	index = "0123456789.";
	// Negativity check
	if (num[0] == '-')
	{
		i = 1;
		isneg = 1;
	}

	else
	{
		i = 0;
		isneg = 0;
	}
	// Check through num for valid number
	for (i=isneg; i<arrlen(num); i++)
	{
		isvalid = 0;
		// Parse index to see if number is valid
		for (j=0; j<arrlen(index); j++)
		{
			if (num[i] == index[j])
			{
				// Matching number here
				isvalid = 1;
			}
		}
		if (!isvalid)
			break;
	}
	if (isneg && isvalid)
		return 2;
	else
		return isvalid;
}
	


int chknumvalid(char num)
{
	// Check to see if single character is a valid number
	// INPUT:	{num}: a possible num character
	// OUTPUT:	in return
	// returns:	Valid/Invalid (1/0)
	
	// Have to convert number
	char *fstr;
	// Exit code
	int excode;

	// Allocate memory
	fstr = new char [2];
	// Format so main chknumvalid function can check string
	fstr[0] = num;
	fstr[1] = '\0';

	// Check
	excode = chknumvalid(fstr);

	// Free memory
	delete [] fstr;

	return excode;
}



int arrless(char *str1, char *str2)
{
	// INPUT:	{str1}, {str2}
	// OUTPUT:	conditional outcome in return
	// returns:	see description
	//
	// DESC:	Check to see if one array is alphabetically smaller than another array
	// 		if str1 = a, str2 = b, return 1
	// 		if str1 = b, str2 = a, return 0

	// Variables
	// lowercase name vars
	char *namea, *nameb;
	// length of names
	int nalen, nblen;
	// smallest length
	int index;
	// loop var
	int i, j;
	// indexes for names
	int nain, nbin;
	// alpha characters
	char *alp;
	int alen;

	// dynamically allocate array
	namea = new char [arrlen(str1)+1];
	nameb = new char [arrlen(str2)+1];
	alp = new char [DEFSIZE];

	// letter index
	arrstore(alp, "0123456789abcdefghijklmnopqrstuvwxyz");
	// Set length of alpha array
	alen = arrlen(alp);
	// Store strings to temporary arrays
	arrstore(namea, str1);
	arrstore(nameb, str2);
	// Convert to lower case
	caseconvert(namea);
	caseconvert(nameb);
	// Get lengths of names
	nalen = arrlen(namea);
	nblen = arrlen(nameb);

	// use smallest name for the index
	if (nalen < nblen)
		index = nalen;
	else
		index = nblen;

	// Do checking up to the smallest name
	for (i=0; i<index; i++)
	{
		// Get index size of first string
		for (j=0; j<alen; j++)
			if (namea[i] == alp[j])
				break;
		nain = j;
		// Get index size of second string
		for (j=0; j<alen; j++)
			if (nameb[i] == alp[j])
				break;
		nbin = j;
		if (nain != nbin)
			break;
	}
	// Check to see if namea is alphabetically lower than nameb
	delete [] alp;
	delete [] namea;
	delete [] nameb;
	if (nain < nbin)
		return 1;
	else if (nain > nbin)
		return 0;
	else
	{
		// They were the same until the end of the loop
		if (nalen <= nblen)
		{
			// semantics since they're nearly the same
			return 1;
		}
		else
			return 0;
	}
	return 0;
}



int arreq(const char *str1, const char *str2)
{
	// Check for array equality
	// INPUT:	{str1}, {str2}
	// OUTPUT:	returns conditional outcome
	// returns:	see description
	//
	// DESC:	if str1 == str2, return (1)
	// 		otherwise return (0)

	// Variables
	// loop var
	int i;
	// length storage
	int stlen1, stlen2;

	// get lengths of strings
	stlen1 = arrlen(str1);
	stlen2 = arrlen(str2);
	// If the lengths aren't equal, the strings aren't
	if (stlen1 != stlen2)
		return 0;
	else
	{
		// Do char by char check to determine equality
		for (i=0; i<stlen1; i++)
		{
			if (str1[i] != str2[i])
			{
				// Not a match
				return 0;
			}
		}
	}
	// Found a match
	return 1;
}



bool arrhas(const char *container, const char *key)
{
	// Check container for existance of key
	int excode;
	excode = arrwhere(container, key);
	if (excode == -1)
		return 0;
	return 1;
	
}



int arrwhere(const char *container, const char *key)
{
	// Check container for existance of key
	// Return location of key in container or -1 if not there
	
	// loop var
	int i;
	// Size of key
	int keysize;
	// Amount matched
	int kchk;

	// initialize values
	kchk = 0;
	keysize = arrlen(key);
	// Run through checking for existence of key
	for (i=0; i<arrlen(container); i++)
	{
		// check through 
		if (container[i] != key[kchk])
		{
			// Char doesn't match
			kchk = 0;
		}
		if (container[i] == key[kchk])
		{
			// Another char match
			kchk++;
			if (kchk == keysize)
			{
				// Found a match
				break;
			}
		}
	}
	// If we found a match, return matched
	if (kchk == keysize)
	{
		// Found a match
		return i-keysize+1;
	}
	return -1;
}



/* Character Storage Functions
 * These functions store one array to another
 */



void arrstore(char *str1, const char *str2, int stlen)
{
	// Store str2 into str1
	// INPUT:	{str1}: Used for output
	// 		{str2}: input array
	// 		{stlen}: Amount to copy
	// OUTPUT:	(arg 2): {stlen} bytes of str2 are copied into str1
	// returns:	void

	// Variables
	// loop var
	int i;

	// Copy
	for (i=0; i<stlen; i++)
	{
		str1[i] = str2[i];
	}
	// Put the end cap on
	str1[i] = '\0';

	return;
}



void arrstore(char *str1, const char *str2)
{
	// Store str2 into str1
	// INPUT:	{str2}: const char array
	// OUTPUT:	{str1}: dynamic char array with contents of {str2}
	// returns:	void
	
	// loop var
	int i;

	// copy array
	for (i=0; i<arrlen(str2); i++)
	{
		str1[i] = str2[i];
	}
	str1[i] = '\0';

	return;
}



void arrappend(char *& destr, const char *str2)
{
	// Appends str2 to destr
	// INPUT:	{destr}: base
	// 		{str2}: addition
	// OUTPUT:	(arg1): new string
	// returns:	void
	
	// variables
	// loop var
	int i;
	// buffer
	char *buffer;
	// Old size of destination string
	int oldsize;
	// Size of appending string
	int appsize;
	// new size of string
	int size;

	oldsize = arrlen(destr);
	appsize = arrlen(str2);
	// put contents of dest str into a buffer
	// so we can resize it
	buffer = new char [oldsize+1];
	arrstore(buffer, destr);
	// reallocate destr
	delete [] destr;
	// Get size of finished str
	size = oldsize + appsize;
	destr = new char [size+1];
	// Put original contents back in destination
	arrstore(destr, buffer);
	delete [] buffer;
	// Now append
	for (i=oldsize; i<size; i++)
		destr[i] = str2[i-oldsize];
	// cap new string
	destr[i] = '\0';

	return;
}



void inttochar(int num, char *ch)
{
	// Takes an integer and converts it to a char array
	
	// loop var
	int i;
	// Sign flag
	int sign;
	// Size|number of digits
	int size;
	// Digit at magnitude
	int power;
	// Isolated digit to store to char
	int digit;
	// Index of numbers for storage
	const char *numindex = "0123456789";
	float numf;

	// Initialize
	sign = 0;

	// Check to see if number is 0
	if (num == 0)
	{
		arrstore(ch, "0");
		return;
	}
	// Check for sign
	if (num < 0)
	{
		// Number is negative
		num = -num;
		ch[0] = '-';
		sign = 1;
	}

	// Get number of digits (length of int)
	numf = (float)num;
	numf = (log(num) / log(10));
	size = numf;
	// Grab all digits and store them to char
	for (i=0; i<=size; i++)
	{
		// start by grabbing
		// current most significant digit
		digit = (int)(num / pow(10, size-i));
		// Store the highest digit to a char array
		ch[i+sign] = numindex[digit];
		// Subtract the digit from the integer
		power = digit * pow(10, size-i);
		num = num - power;

	}
	// Cap the new char
	ch[i+sign] = '\0';


}



int arstrf(char *stream, int size, const char *message, ...)
{
	// Replacement for sprintf()
	// Uses variable arguments
	// INPUT:	{size}:	can be 0 (no overflow check, unsafe),
	// 			or the size of stream (safe)
	// 		{message}: formatted message to store to stream
	// OUTPUT:	{stream}: message with formatting replaced
	// returns:	error (overflow) or size of stream (-1/size)
	
	// Variables
	// loop vars
	int i, j;
	// Placeholder for storage
	int index;
	// Argument flag
	bool arg;
	// Error flag
	bool overflow = 0;
	// Integer buffer
	int inum;
	// string buffer
	char *buffer;
	// message buffer (so we don't modify message)
	char *msg;
	// List of arguments
	va_list vargs;

	// Allocate memory for message buffer
	msg = new char [arrlen(message)+1];
	// Store message to message buffer
	arrstore(msg, message);
	// Start processing through argument list
	va_start(vargs, message);

	// Start storage at 0
	index = 0;
	// No arguments yet
	arg = 0;
	// Look through message a char at a time
	for (i=0; i<arrlen(msg); i++)
	{
		if (msg[i] != '%' && !arg)
		{
			// Not an argument modifier
			stream[index] = msg[i];
			// Increment index
			index++;
		}
		else if (msg[i] == '%' && arg)
		{
			// input is %%
			stream[index] = msg[i];
			// Increment index
			index++;
			arg = 0;
		}
		else if (msg[i] == '%' && !arg)
		{
			// formatting code is next
			arg = 1;
		}
		else if (arg && msg[i] == 'c')
		{
			// input %c (character)
			stream[index] = va_arg(vargs, int);
			// Increment index
			index++;
			arg = 0;
		}
		else if (arg && msg[i] == 'i')
		{
			// input %i (integer)
			inum = va_arg(vargs, int);
			buffer = new char [20];
			// Convert integer to character
			inttochar(inum, buffer);
			// Overflow check
			if (index + arrlen(buffer) >= size && size)
			{
				// overflow error
				overflow = 1;
				delete [] buffer;
				break;
			}
			// Add to index
			for (j=index; j<arrlen(buffer)+index; j++)
				stream[j] = buffer[j-index];
			// Increment index
			index = index + arrlen(buffer);
			// free memory that isn't needed anymore
			delete [] buffer;
			arg = 0;
		}
		else if (arg && msg[i] == 's')
		{
			// input %s (string)
			buffer = va_arg(vargs, char*);
			// overflow check
			if (index + arrlen(buffer) > size && size)
			{
				// overflow error
				overflow = 1;
				break;
			}
			// append to stream
			for (j=index; j<arrlen(buffer)+index; j++)
				stream[j] = buffer[j-index];
			// Increment index
			index = index + arrlen(buffer);
			arg = 0;
		}
		else if (arg)
		{
			// Unknown error
			break;
		}
		// Overflow check
		if (index >= size && size)
		{
			// Overflow error
			overflow = 1;
			break;
		}
	}
	// Finish variable arguments
	va_end(vargs);

	// Free allocated memory
	delete [] msg;
	// Check for overflow
	if (overflow)
	{
		// Return failure if overflow
		return -1;
	}
	else if (arg)
	{
		// Input bad formatting char
		return -2;
	}
	// Cap the end of the stream
	stream[index] = '\0';

	// Return success
	return index;
}



/* Character Formatting Functions
 * These functions modify character arrays
 * Usually used for the loading procedure
 */



void caseconvert(char *myinput)
{
	// convert any uppercase to lowercase in {myinput}
	// INPUT:	{myinput}: input string
	// OUTPUT:	(arg 1): modified
	// returns:	void
	
	// Case checking variables
	char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char lower[] = "abcdefghijklmnopqrstuvwxyz";
	// loop vars
	int i, j;
	// lengths
	int inputlen = arrlen(myinput);
	int abclen = arrlen(lower);

	// Loop through the string
	for (i = 0; i < inputlen; i++)
	{	// Check each char in myinput for uppercase
		for (j = 0; j < abclen; j++)
		{
			// check char by char for uppercase
			if (myinput[i] == upper[j])
				myinput[i] = lower[j];
		} 
	} 

	return;
}



int cutfromindex(char *myin, int index)
{
	// cuts from index point to end
	// INPUT:	{myin}: input string
	// 		{index}: new beginning of {myin}
	// OUTPUT:	(arg 1): newly cut string
	// returns:	new size of {myin}
	
	// Variables
	// loop var
	int i;
	// Size of input string
	int myinlen;

	// Get size of input string
	myinlen = arrlen(myin);
	myinlen -= index;
	// move everything right of index to beginning of string
	for (i=0; i<myinlen; i++)
		myin[i] = myin[i+index];
	// Add new cap
	myin[i] = '\0';

	return myinlen;
}



int keepxcut(char *myin, char c)
{
	// Use string from beginning to char
	// Variables
	int i;
	for (i=0; i<arrlen(myin); i++)
	{
		if (myin[i] == c)
			break;
	}
	myin[i] = '\0';

	return i;
}



int cutxkeep(char *myin, char c)
{
	// Use string after instance of char
	// loop vars
	int i, j;
	// Find the first occurrance of char
	for (i=0; i<arrlen(myin); i++)
	{
		if (myin[i] == c)
			break;
	}
	// If we hit the end, return
	if (i == arrlen(myin))
		return -1;
	i++;
	// Don't keep pivot char, keep everything after it
	j = 0;
	for (i=i; i<arrlen(myin); i++)
	{
		myin[j] = myin[i];
		j++;
	}
	// cap, and return size
	myin[j] = '\0';
	return j;
}



/* Time Functions
 * Use the time.h library
 */



int formatCtime(char *date)
{
	// timestamping
	// INPUT:	{date}: input string
	// OUTPUT:	(arg 1): current time formatted
	// returns:	0 (always)
	//
	// DESC:	This will get a timestamp from the system,
	// 		format it for the program, and return that formatted string
	
	// Variables
	// current time
	time_t currentTime;
	// localtime pointer
	struct tm *ptm;
	
	// Get current time
	time(&currentTime);
	ptm = localtime(&currentTime);
	// Format time for program usage, store in date
	strftime(date, DEFSIZE, "%m %d %Y %H %M", ptm);
	
	return 0;
}



// Standard I/O Functions
// Print to console



int ch_color(const char *color)
{
	// Change the color of text
	// INPUT:	Color name, attribute (normal, bold)
	// OUTPUT:	Change color on the terminal
	// returns:	color change success/failure (0/-1)
	
	// Indicators for easier reading and debugging
	enum indicator_no
	{
	    C_LEFT, C_RIGHT, C_RESET, 
	    C_BLACK, C_BLUE, C_GREEN, C_CYAN, C_RED, C_PURPLE, C_BROWN,
	    C_LGRAY, C_DGRAY, C_LBLUE, C_LGREEN, C_LCYAN, C_LRED, C_LPURPLE,
	    C_YELLOW, C_WHITE, C_CLR
	};
	// Color Table: List of escape codes to change colors
	char **coltable;
	// Color Index: conversion from color char to number
	char **colindex;
	// Loop var
	int i;
	// Number of table entries
	const int num = 22;
	// Integer selection
	int select;

	if (!USE_COLOR)
	{
		// If color isn't wanted, just return
		return 0;
	}

	// Allocate memory
	coltable = new char * [num];
	colindex = new char * [num];
	for (i=0; i<num; i++)
	{
		// Continue allocating and initializing
		coltable[i] = new char [10];
		colindex[i] = new char [15];
		coltable[i][0] = '\0';
		colindex[i][0] = '\0';
	}
	// Make color conversion table: color-->escape code
	arrstore(colindex[C_RIGHT],	"normal");
	arrstore(colindex[C_RESET],	"reset");
	arrstore(colindex[C_BLACK],	"black");
	arrstore(colindex[C_BLUE],	"blue");
	arrstore(colindex[C_GREEN],	"green");
	arrstore(colindex[C_CYAN],	"cyan");
	arrstore(colindex[C_RED],	"red");
	arrstore(colindex[C_PURPLE],	"purple");
	arrstore(colindex[C_BROWN],	"brown");
	arrstore(colindex[C_LGRAY],	"light gray");
	arrstore(colindex[C_DGRAY],	"dark gray");
	arrstore(colindex[C_LBLUE],	"light blue");
	arrstore(colindex[C_LGREEN],	"light green");
	arrstore(colindex[C_LCYAN],	"light cyan");
	arrstore(colindex[C_LRED],	"light red");
	arrstore(colindex[C_LPURPLE],	"light purple");
	arrstore(colindex[C_YELLOW],	"light yellow");
	arrstore(colindex[C_WHITE],	"white");
	arrstore(colindex[C_CLR],	"clear");
	// Make color code table with escape codes
	arrstore(coltable[C_LEFT],	"\033[");	
	arrstore(coltable[C_RIGHT],	"m");	
	arrstore(coltable[C_RESET],	"0");
	arrstore(coltable[C_BLACK],	"0;30");
	arrstore(coltable[C_BLUE],	"0;34");
	arrstore(coltable[C_GREEN],	"0;32");
	arrstore(coltable[C_CYAN],	"0;36");
	arrstore(coltable[C_RED],	"0;31");
	arrstore(coltable[C_PURPLE],	"0;35");
	arrstore(coltable[C_BROWN],	"0;33");
	arrstore(coltable[C_LGRAY],	"0;37");
	arrstore(coltable[C_DGRAY],	"1;30");
	arrstore(coltable[C_LBLUE],	"1;34");
	arrstore(coltable[C_LGREEN],	"1;32");
	arrstore(coltable[C_LCYAN],	"1;36");
	arrstore(coltable[C_LRED],	"1;31");
	arrstore(coltable[C_LPURPLE],	"1;35");
	arrstore(coltable[C_YELLOW],	"1;33");
	arrstore(coltable[C_WHITE],	"1;37");
	arrstore(coltable[C_CLR],	"\033[K");
	// Find color selection
	select = 0;
	for (i=0; i<num; i++)
	{
		// If color was specified, give integer value
		if (arreq(color, colindex[i]))
			select = i;
		delete [] colindex[i];
	}
	delete [] colindex;
	// Change the color
	cout << coltable[C_LEFT] << coltable[select] << coltable[C_RIGHT];
	// Free up remaining used memory
	for (i=0; i<num; i++)
		delete [] coltable[i];
	delete [] coltable;
	// Return success
	return 0;
}



int fdialog(queue *fifo)
{
	// General input wrapper
	// INPUT:	fifo: message about input
	// OUTPUT:	fifo: user input stored in fifo
	// returns:	Valid/Invalid general input (0/1)
	
	// Temporary char array
	char *ctemp;
	char *tchar;
	// incrementors
	int subinc;
	int incrementor;
	// Size used for input
	int size = 5;
	// Input chars
	char ch, ahead;
	// Buffer queue
	queue *qstr;
	// Check for operator
	int iop;
	// Color flag
	bool used_color = 0;
	// Exit code
	int excode;

	// Allocate dummy block
	ctemp = new char [1];
	cout << endl;
	// Grab fifo data, checking operators as well
	// Valid operators:	1:	Set size of input stream
	while (fifo->has_data())
	{
		// Dequeue next part to read
		iop = fifo->dequeue(ctemp);
		if (iop == 1)
		{
			// Specifying a size for input array
			if (chknumvalid(ctemp))
			{
				// Use as sizing for array
				// if the size is sane
				if (atoi(ctemp) > 0)
				{
					size = atoi(ctemp);
				}
			}
		}
		else if (iop == 2)
		{
			// Specifying a color
			ch_color(ctemp);
			used_color = 1;
		}
		else
		{
			cout << ctemp;
			if (fifo->has_data())
				cout << endl;
			if (used_color)
			{
				// Reset color first
				ch_color("reset");
				used_color = 0;
			}
		}
	}
	// Set up to read value from user
	delete [] ctemp;
	ctemp = new char [size];
	qstr = new queue;
	subinc = 0;
	incrementor = 0;
	do {
		// Grab everything from the stream
		cin.get(ch);
		ctemp[subinc] = ch;
		subinc++;
		incrementor++;
		// Check to see if there's more data
		// in the stream
		if (ch != '\n' && ch)
			ahead = cin.peek();
		else
			ahead = '\n';
		if (subinc > size-2)
		{
			// Out of space in input var
			// queue current data
			ctemp[subinc] = '\0';
			qstr->enqueue(ctemp);
			subinc = 0;
		}
	} while (ahead != '\n' && ch != '\n');
	if (ch != '\n')
	{
		// Clear stream if we need to
		cin.clear();
		cin.ignore();
	}
	// queue up final chunk of stream
	ctemp[subinc] = '\0';
	qstr->enqueue(ctemp);
	// Re-init input var
	delete [] ctemp;
	ctemp = new char [incrementor+1];
	ctemp[0] = '\0';
	// Init queue buffer
	tchar = new char [1];
	// cat queue into input var
	while (qstr->has_data())
	{
		// Append queue data to input
		qstr->dequeue(tchar);
		arrappend(ctemp, tchar);
	}

	//cin.getline(ctemp, size, '\n');
	cout << ctemp << endl;
	if (ctemp[0] == '\n')
		ctemp[0] = 0;
	fifo->enqueue(ctemp);
	
	// If user didn't input anything, return empty
	if (!ctemp[0])
	{
		excode = 0;
	}
	else
		excode = 1;
	delete [] ctemp;
	delete [] tchar;
	delete qstr;
	return excode;
}



void qError(char errinfo[])
{	
	// Error Dialog
	// INPUT:	{errinfo[]}: a short message about the error
	// OUTPUT:	PRINT an error message to STDOUT
	// returns:	void
	//
	// DESC:	Display an error message, wait for user to press [Return]
	
	cout << endl << endl;
	cout << "Error: " << errinfo << endl;
	cout << "Press [Return] to continue:" << endl;
	cin.ignore();

	return;
}



void qError(void)
{	
	// Error Dialog
	// INPUT:	void
	// OUTPUT:	PRINT an error message to STDOUT
	// returns: 	void
	//
	// DESC:	Display a general error message, wait for
	// 		user to press [Return]

	cout << endl << endl;
	cout << "Error: Unknown" << endl;
	cout << "Press [Return] to continue:" << endl;
	cin.ignore();

	return;
}



// Portable Menu Code
int menu(queue *fifo, queue *context)
{
	// Revamped menu function that only uses queues
	// INPUT:	fifo: menu entries
	// 		context: context for the entries
	// OUTPUT:	PRINTS fifo and context to STDOUT,
	// 		takes user input, sends user input in return
	// returns:	User input
	
	// loop var
	int i;
	// Number of entries
	int num;
	// lowercase char
	char *lower;
	// temporary storage char
	char *ctemp;
	char *csel;
	char *tchar;
	// Bar length
	int barlength;
	int contnum;
	// User selection
	int mysel;
	int hiddensel;
	// general size
	int size;
	// Size of first column
	int colsize;
	// Exit code
	int excode = 0;
	int ecode;
	// Number of menu entries:
	// Used to impl hidden entries
	int mennum;
	// Non-unique entry handling
	queue *fdup;
	queue *cdup;
	int matches;
	int hidden_matches;

	// Add some space padding
	for (i=0; i<8; i++)
		cout << endl;
	// Get number of entries
	num = fifo->qlen();

	// Give ctemp some memory
	ctemp = new char [1];

	// Get sizing of fifo without title
	colsize = 0;
	fifo->rotate();
	// Grab each entry, check size
	while (fifo->rotate(ctemp) != 1)
	{
		size = arrlen(ctemp);
		// Check to see if there is a new max
		if (size > colsize)
		{
			colsize = size;
		}
	}

	do
	{
		// Get number of entries
		mennum = fifo->qlen();
		// Display Title
		// Reset rotation pointers
		fifo->rotate_reset();
		context->rotate_reset();
		// Grab title
		fifo->rotate(ctemp);
		// Use color if specified
		ch_color("light green");
		cout << ctemp << endl;
		ch_color("reset");
		// Check to see how long our bars should be
		barlength = colsize + context->max() + 40;
		// Make the bar, use color if specified
		ch_color("dark gray");
		for (i=0; i<barlength; i++)
			cout << "-";
		ch_color("reset");
		cout << endl;
		num = 1;
		excode = 0;
		// Grab number of context entries
		contnum = context->qlen();
		while (!excode)
		{
			// Display all menu items
			// exit codes:
			// 	1: use a different color
			// 	2: menu is hidden

			ecode = fifo->getrotop();
			excode = fifo->rotate(ctemp);
			if (ecode != 2)
			{
				// Show the entry
				cout << '\t' << num << "|\t";
				num++;
				if (ecode == 1)
				{
					// Different color
					ch_color(ctemp);
				}
				else
				ch_color("blue");
				cout << ctemp;
				ch_color("reset");
			}
			else
			{
				// Hidden Option
				// Decrement entry number
				mennum--;
			}
			if (context->has_data())
			{
				if (contnum)
				{
					// Has a context menu
					// Fancy formatting
					// Get number of tabs needed
					size = colsize/8 - arrlen(ctemp)/8;
					//if (len(ctemp) < 8)
					for (i=0; i<=size; i++)
						cout << '\t';
					context->rotate(ctemp);
					ch_color("dark gray");
					cout << '\t' << ctemp;
					ch_color("reset");
					contnum--;
				}
			}
			cout << endl;
		}

		// Give the user a prompt
		for (i=0; i<4; i++)
			cout << endl;
		ch_color("dark gray");
		for (i=0; i<barlength; i++)
			cout << "-";
		ch_color("reset");
		cout << endl;
		delete [] ctemp;
		ctemp = new char [1];
		csel = new char [DEFSIZE];
		cout << "Make a Selection: ";
		cin.getline(csel, DEFSIZE, '\n');


		// Interpret user selection
		if (!csel[0])
		{
			// User didn't input anything
			cout << "default" << endl;
			delete [] ctemp;
			delete [] csel;
			return 0;
		}
		// Check to see if input is a number
		if (chknumvalid(csel))
		{
			// Input is a number
			mysel = atoi(csel);
			num = mysel;
			fifo->rotate_reset();
			while (num > 0)
			{
				// Get selection
				fifo->rotate();
				num--;
			}
			fifo->rotate(ctemp);
			// Display user selection
			if (mysel > 0 && mysel < mennum)
				cout << ctemp << endl;
			if (mysel > 0 && mysel < mennum)
			{
				// Make sure number is valid
				delete [] csel;
				delete [] ctemp;
				return mysel;
			}
		}

		// Look for full name matches
		// init selection variables
		mysel = 0;
		hiddensel = 0;
		// Allocate dup handlers
		fdup = new queue;
		cdup = new queue;
		matches = 0;
		hidden_matches = 0;
		// Store title in dup handler
		fifo->rotate_reset();
		fifo->rotate(ctemp);
		fdup->enqueue(ctemp);
		// Convert user selection to lowercase
		caseconvert(csel);
		// Initialize some variables
		// starting at entry 1 (no title)
		num = 1;
		// No user input yet
		mysel = 0;
		// variable to check for multiple matches
		i = 0;
		// Get number of context entries:
		// Used for duplicate handline
		contnum = context->qlen();
		// Do character checking
		do {
			// Loop through fifo looking for match
			ecode = fifo->getrotop();
			excode = fifo->rotate(ctemp);
			tchar = new char [1];
			if (contnum)
			{
				context->rotate(tchar);
				contnum--;
			}
			else
				tchar[0] = '\0';
			// convert to lowercase for checking
			lower = new char [arrlen(ctemp)+1];
			// Convert entry to lowercase for checking
			arrstore(lower, ctemp);
			caseconvert(lower);
			if (arrhas(lower, csel))
			{
				// Found a match
				// Decide if match is "hidden" or not
				if (ecode == 2)
				{
					// Hidden entry
					// Don't put into dup check
					cout << ctemp << endl;
					hidden_matches++;
					hiddensel = num;
				}
				else
				{
					// Non-hidden entry
					// Load onto queue for dup check
					matches++;
					// Set user selection
					mysel = num;
					cout << ctemp << endl;
					fdup->enqueue(ctemp);
					if (tchar[0])
						cdup->enqueue(tchar);
					else
						cdup->enqueue("\0");
					cdup->setop(num);
				}
			}
			num++;
			delete [] lower;
			delete [] tchar;
		} while (!excode);

		// Evaluate
		if (!mysel && !hiddensel)
		{
			// No user selection
			// Tell the user
			// Allocate to new size
			delete [] ctemp;
			size = arrlen("no match for ") + 2 + arrlen(csel);
			ctemp = new char [size];
			// Pass message
			arstrf(ctemp, size, "No match for %s", csel);
			qError(ctemp);
		}
		else if (matches > 1)
		{
			// Duplicates found
			fdup->enqueue("back");
			mysel = menu(fdup, cdup);
			if (mysel == matches+1)
			{
				// User selected back
				// don't do anything
			}
			else if (mysel == 0)
			{
				// User selected default
				// Go back
				matches = 1;
			}
			else
			{
				// Have to get choice from context
				cdup->rotate_reset();
				while (mysel)
				{
					num = cdup->getrotop();
					cdup->rotate();
					mysel--;
				}
				mysel = num;
				matches = 1;
			}
		}
		else if (!matches && hidden_matches)
		{
			// Found a hidden match
			matches = 1;
			mysel = hiddensel;
		}
		// Deallocate memory
		delete [] csel;
		delete cdup;
		delete fdup;
		// Keep looping until only 1 possibility is given
	} while (matches != 1);
	delete [] ctemp;
	return mysel;
}



int menu(queue *fifo)
{
	// Menu with no context
	queue *context;
	int mysel;
	context = new queue;
	mysel = menu(fifo, context);
	delete context;
	return mysel;
}



// File I/O Functions
// Read and Write from file



int utilLoad(queue *fifo, char *fname)
{
	// Serial Loading Function
	// INPUT:	{fifo}: command index
	// 		{fname}: file to READ from
	// OUTPUT:	(arg 1): old data is dumped and data from {fname} is added
	// returns:	Execution Success/Failure (0/1)
	
	// DESC:	Takes operations from fifo and matches them to
	// 		operations given in loaded file. Queues data from file and 
	// 		uses op info to set 
	// 		operation numbers in fifo.
	// 		Designed for modularity
	
	// Variables
	// loop var
	int i;
	// Temporary value
	char *tchar;
	// Temporary name 
	char *tname;
	// Flag for read error
	bool rErr = 0;
	// Flag to indicate end of file
	bool eof = 0;
	// Exit code
	int excode;
	// File reading variable
	ifstream inData(fname);
	// Error info (only initialized if needed)
	char *errinfo;
	// Class operations
	char **fileop;
	// Number of operations
	int opnum;
	
	// Open the file if we can
	if (!inData.good())
	{
		// File does not exist
		errinfo = new char [DEFSIZE];
		arrstore(errinfo, "ERROR: file does not exist!");
		qError(errinfo);
		delete [] errinfo;
		return 1;
	}
	// Get operations and index them
	opnum = fifo->qlen()+1;
	fileop = new char * [opnum+1];
	for (i=0; i<opnum+1; i++)
		fileop[i] = new char [1];
	tchar = new char [1];
	while (fifo->has_data())
	{
		// Put all operations into an operation array
		excode = fifo->dequeue(tchar);
		if (excode > 0 && excode < opnum)
		{
			// Valid
			delete [] fileop[excode];
			fileop[excode] = new char [arrlen(tchar)+1];
			arrstore(fileop[excode], tchar);
		}
		else if (!excode)
		{
			// Comment, maybe I'll do something here later?
		}
		else
		{
			// Invalid
			rErr = 1;
		}
	}
	delete [] tchar;
	// Read in the rest of the file
	while (!inData.eof() && !rErr && !eof)
	{
		// Grab a line from the file and
		// store it to tchar
		tchar = new char [1];
		tchar[0] = '\0';
		eof = grabline(tchar, inData);
		if (!inData.eof() && !eof)
		{
			// Grab the operation and argument
			tname = new char [arrlen(tchar)];
			excode = stripLine(tchar, tname);
			for (i=1; i<opnum; i++)
			{
				if (arreq(fileop[i], tchar))
				{
					// store with command code to fifo
					fifo->enqueue(tname);
					fifo->setop(i);
				}
			}
			if (excode)
			{
				// Something happened, exit with errors
				rErr = 1;
			}
			delete [] tname;
		}
		delete [] tchar;
	}
	// Deallocate and return exit success or failure
	for (i=0; i<opnum+1; i++)
	{
		delete [] fileop[i];
	}
	delete [] fileop;
	if (!rErr)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



int utilSave(queue *fifo, queue *operands, char *fname)
{
	// Serial Save function
	// INPUT:	{fifo}: Data to write to external file
	// 		{operands}: Operations to link with op numbers from fifo
	// 		{fname}: file to WRITE to
	// OUTPUT:	NONE
	// returns:	Execution Success/Failure (0/1)
	
	// DESC:	Takes data from the fifo and writes it to an external
	// 		file, matching operands to op numbers. Designed to
	// 		be highly modular

	// Read Variable
	ofstream odat;
	// loop vars
	int i;
	// Error info
	char *errinfo;
	// operations
	char **op;
	char *tchar;
	int opnum;
	int excode;

	// Error-check
	if (!fname[0])
	{
		// If there's no file name to save to,
		// give user error and stop
		errinfo = new char [DEFSIZE];
		arrstore(errinfo, "Bad file name. File was not saved");
		qError(errinfo);
		delete [] errinfo;
		return 1;
	}

	opnum = operands->qlen()+1;
	// Allocate
	op = new char * [opnum];
	for (i=0; i<opnum; i++)
	{
		op[i] = new char [1];
	}
	// Store information from operands to operations
	tchar = new char [1];
	while (operands->has_data())
	{
		// Grab the next operation
		excode = operands->dequeue(tchar);
		if (excode >= 0 && excode < opnum)
		{
			// Found a valid operation, add it to list
			delete [] op[excode];
			op[excode] = new char [arrlen(tchar)+1];
			arrstore(op[excode], tchar);
		}

	}
	// Open file for Writing
	odat.open(fname);
	// Save file to database
	while (fifo->has_data())
	{
		excode = fifo->dequeue(tchar);
		if (excode && excode < opnum)
		{
			// Valid Data
			if (excode == 1)
				odat << endl;
			odat << op[excode] << ": ";
			odat << tchar << endl;
		}
		else if (!excode)
		{
			// comment
			odat << "# " << tchar << endl;
		}

	}
	// Done. Delete allocated variables
	for (i=0; i<opnum; i++)
		delete [] op[i];
	delete [] op;
	delete [] tchar;

	// Close the file
	odat.close();
	
	return 0;
}



int stripLine(char *ops, char *&targ)
{
	// getline formatting
	// INPUT:	{ops}: input character
	// 		{targ}: used for output
	// OUTPUT:	(arg 1): Operation found in line
	// 		(arg 2): Argument found in line
	// returns:	0 (always)
	//
	// DESC:	takes a line provided by grabline
	// 		and splits it into the opcode and the argument
	// 		Return 0 if successful, 1 if error

	// Variables
	// line stored here
	char *fullLine;
	int size;

	delete [] targ;
	size = arrlen(ops)+1;
	fullLine = new char [size];
	targ = new char [size];
	arrstore (fullLine, ops);

	// Grab just the opcode
	//useToColon(ops);
	keepxcut(ops, ':');
	arrstore (targ, fullLine);

	// Grab the argument
	//useAfterColon(targ);
	cutxkeep(targ, ':');
	if (targ[0] == ' ')
		cutxkeep(targ, ' ');

	delete [] fullLine;
	

	return 0;
}



bool grabline (char *&str, ifstream &inData)
{
	// External File Parsing
	// INPUT:	{str}: output var
	// 		{inData}: file pointer
	// OUTPUT:	(arg 1): line from file stream
	// returns:	line is Valid/Invalid (0/1)
	//
	// DESC:	Grab a line from an external file as long as it's not the EOF
	// 		Check to see if data is valid (not newline, comment, etc)

	// Variables
	// Size modifier for input size
	int streammod = 40;
	// Validity Flag
	bool isValid = 0;
	int size;
	// Stream position
	char *buffer;
	char c;
	int index = 0;


	// Reallocate stream
	delete [] str;
	str = new char [streammod+1];
	// Go until we hit the end of file
	while (!inData.eof() && !isValid)
	{
		// Set initial size
		size = streammod;
		index = 0;
		// Start by saying this is valid until proven wrong
		isValid=1;
		// Grab a line from the external file until we hit the delimeter.
		// Grow the read array if necessary
		do {
			inData.get(c);
			if (inData.eof())
			{
				str[index] = '\0';
				index++;
				break;
			}
			else if (c != '\n')
			{
				str[index] = c;
				index++;
			}
			else
			{
				str[index] = '\0';
				index++;
				break;
			}
			if (index >= size/2)
			{
				str[index] = '\0';
				buffer = new char [size+streammod];
				streammod += streammod / 2;
				arrstore(buffer, str);
				// Delete stream
				delete [] str;
				// Point to buffer
				str = buffer;
				// Point buffer to empty
				buffer = 0;
				// Change size accordingly
				size = size + streammod;
				if (size > 1000)
				{
					cout << "error" << endl;
					return 1;
				}
			}
		} while (!inData.eof());

		// Check to see if it's valid
		// Check for comments
		if (str[0] == '#')
			isValid = 0;
		// Check for spaces
		if (str[0] == ' ')
			isValid = 0;
		// Check for weird stuff
		if (!str[0])
			isValid = 0;
		// If we made it past the conditions, line is valid
		if (isValid)
			break;
		// If not, keep going
	}
	// We hit the end of the file weirdly
	if (!isValid)
		return 1;
	// We did not hit the end of file weirdly
	return 0;
}
