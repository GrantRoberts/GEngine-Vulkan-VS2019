#pragma once
#include <assert.h>
#include <iostream>

template<typename T>
// The Dynamic Array class.
class DynamicArray
{
public:
	//---------------------------------------------------------------------------------------
	// Create the array, initialising the variables.
	// Params: the size of the array - defaults to 1.
	//---------------------------------------------------------------------------------------
	DynamicArray(int size = 1)
	{
		// Initialize the size of the array.
		mArraySize = size;

		// If the size of the array was entered to be less than 1, it is assigned to 1. (For safety, in case someone enters a negative number)
		if (mArraySize < 1)
		{
			mArraySize = 1;
		}

		// Create the new array
		mDataPtr = new T[mArraySize];

		// The next empty space is the first space in the array.
		mCurrentData = 0;
	}

	//---------------------------------------------------------------------------------------
	// Delete the array.
	//---------------------------------------------------------------------------------------
	~DynamicArray()
	{
		delete mDataPtr;
		mDataPtr = nullptr;
	}

	//---------------------------------------------------------------------------------------
	// Algorithm: Bubble Sort
	// Input: An array of data T, operator for comparison O
	//
	// For each item in T, do			-How many times the sort has been looped
	//		For each item in T, do		-Where in the array we are checking
	//			if O is >, then
	//				if T[j] > T[j + 1], then
	//					temp = T[j]
	//					T[j] = T[j + 1]
	//					T[j + 1] = temp
	//			if O is <, then
	//				if T[j] < T[j + 1], then
	//					temp = T[j + 1]
	//					T[j + 1] = T[j]
	//					T[j] = temp
	//---------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------
	// Sorts the array in the determined order using Bubble Sorting.
	// WARNING: Will sort based on numerical order, characters will be sorted by ASCII!
	// Parmas: char of operator for comparing the data. (eg. < = decending, > = ascending)
	//---------------------------------------------------------------------------------------
	void Sort(char opr)
	{
		// The number of times the list has been sorted.
		for (int i = 0; i < mCurrentData - 1; ++i)
		{
			// Where in the array we are checking, ignoring items that have been sorted.
			for (int j = 0; j < mCurrentData - 1 - i; ++j)
			{
				// Temporary variable for storage.
				T temp;
				switch (opr)
				{
					// Decending order.
					case '<':
						if (mDataPtr[j] < mDataPtr[j + 1])
						{
							temp = mDataPtr[j];
							mDataPtr[j] = mDataPtr[j + 1];
							mDataPtr[j + 1] = temp;
						}
						break;
					// Ascending order.
					case '>':
						if (mDataPtr[j] > mDataPtr[j + 1])
						{
							temp = mDataPtr[j + 1];
							mDataPtr[j + 1] = mDataPtr[j];
							mDataPtr[j] = temp;
						}
						break;
					// If the parameter is invalid.
					default:
						std::cout << "INVALID PARAMETER FOR SORTING FUNCTION!" << std::endl;
						return;
				}
			}
		}
	}

	//---------------------------------------------------------------------------------------
	// Add a value to the start of the array.
	// Params: the data to be added.
	//---------------------------------------------------------------------------------------
	void AddFront(T data)
	{
		Resize();

		// The data in the array are moved forward one
		for (int i = mCurrentData; i > 0; --i)
		{
			mDataPtr[i] = mDataPtr[i - 1];
		}

		// The first element in the array becomes the data entered
		mDataPtr[0] = data;

		// The next empty element in the array increases.
		++mCurrentData;
	}

	//---------------------------------------------------------------------------------------
	// Add a value to the end of the array.
	// Params: the data to be added.
	//---------------------------------------------------------------------------------------
	void AddBack(T data)
	{
		Resize();

		// The next empty element in the array has the data assigned.
		mDataPtr[mCurrentData] = data;

		// The next empty element in the array increases.
		++mCurrentData;
	}

	//---------------------------------------------------------------------------------------
	// Remove the value at the start of the array.
	//---------------------------------------------------------------------------------------
	void RemoveFront()
	{
		// The other elements in the array are moved back one.
		for (int i = 0; i < (mCurrentData - 1); ++i)
		{
			mDataPtr[i] = mDataPtr[i + 1];
		}

		// The next empty element in the array decreases.
		--mCurrentData;
	}

	//---------------------------------------------------------------------------------------
	// Remove the value at the back of the array.
	//---------------------------------------------------------------------------------------
	void RemoveBack()
	{
		// The next empty element in the array increases.
		--mCurrentData;
	}

	//---------------------------------------------------------------------------------------
	// Insert new data to somewhere inside the array other than the end.
	// Params: the index of where the data is to be placed, the data to be placed.
	//---------------------------------------------------------------------------------------
	void Insert(int index, T data)
	{
		Resize();

		// The next empty element in the array increases.
		++mCurrentData;

		// Make an empty space for the new data where it has been specified.
		for (int i = mCurrentData; i > index; --i)
		{
			mDataPtr[i] = mDataPtr[i - 1];
		}

		// The selected element index becomes the data that was input.
		mDataPtr[index] = data;
	}

	//---------------------------------------------------------------------------------------
	// Erases an element by its index.
	// Params: the index of the element you want to delete.
	//---------------------------------------------------------------------------------------
	void Erase(int index)
	{
		// Move all the elements in the array back 1, overwriting the element at the desired index, effectively erasing it.
		for (int i = index; i < mCurrentData; ++i)
		{
			mDataPtr[i] = mDataPtr[i + 1];
		}

		// The next empty element in the array decreases.
		--mCurrentData;
	}

	//---------------------------------------------------------------------------------------
	// Check if the array is empty.
	// Returns: true if empty, else false.
	//---------------------------------------------------------------------------------------
	bool Empty()
	{
		// If the next empty space in the array is the first element of the array: it is empty.
		if (mCurrentData == 0)
			return true;
		// Else there is data in the array.
		return false;
	}

	//---------------------------------------------------------------------------------------
	// How many elements there are in the array.
	// Returns: the amount of elements in the array.
	//---------------------------------------------------------------------------------------
	int Size()
	{
		// mCurrentData = the index of the next empty space, which also equals the size of the array (As arrays start at 0)
		return mCurrentData;
	}

	//---------------------------------------------------------------------------------------
	// Remove all elements in the list.
	//---------------------------------------------------------------------------------------
	void Clear()
	{
		// Set the next "empty" element to be the first element, adding will rewrite
		mCurrentData = 0;
	}

	//---------------------------------------------------------------------------------------
	// Algorithm: Search for data
	// Input: the data to be found D
	//
	// For each item in the array, do
	//		if item == D, then
	//			return item
	//		else
	//			check the next item
	//---------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------
	// Finds the element with the entered data.
	// Params: the data to be found.
	// Returns: the data that was found.
	//---------------------------------------------------------------------------------------
	T Find(T data)
	{
		for (int i = 0; i < mArraySize; ++i)
		{
			if (mDataPtr[i] == data)
				return mDataPtr[i];
		}
	}

	//---------------------------------------------------------------------------------------
	// Removes all elements in the array that are equal to the data.
	// Params: the data that is to be removed from the array.
	//---------------------------------------------------------------------------------------
	void Remove(T data)
	{
		int arraySize = mArraySize;
		// Loops through the array.
		for (int i = 0; i < arraySize;)
		{
			// If the element and the data to be removed are NOT the same: check the next element in the array.
			if (mDataPtr[i] != data)
				++i;		// So if the element that was previously checked was deleted, we don't move past the next item.
			else
			{
				// Move the other elements over the data from the checked index.
				for (int j = i; j < arraySize; ++j)
				{
					mDataPtr[j] = mDataPtr[j + 1];
				}
				// Decrease the size of the array as an element was removed
				--mCurrentData;
			}
		}
	}

	//---------------------------------------------------------------------------------------
	// Returns the first element in the array.
	// Returns: the first element in the array.
	//---------------------------------------------------------------------------------------
	T First()
	{
		return mDataPtr[0];
	}

	//---------------------------------------------------------------------------------------
	// Returns the last element in the array.
	// Returns: the last element in the array.
	//---------------------------------------------------------------------------------------
	T Last()
	{
		return mDataPtr[mCurrentData - 1];
	}

	//---------------------------------------------------------------------------------------
	// Get the data as an array
	// Returns: the dynamic array's data as an array.
	//---------------------------------------------------------------------------------------
	T* Data()
	{
		return mDataPtr;
	}

	//---------------------------------------------------------------------------------------
	// For using [] for the array.
	//---------------------------------------------------------------------------------------
	T& operator[](int index)
	{
		// Assert to not access a negative index.
		assert(index >= 0 && "Index was too low");
		// Assert to not try to access something outside the size of the array.
		assert(index < mArraySize && "Index was too large");
		return mDataPtr[index];
	}

private:
	//---------------------------------------------------------------------------------------
	// Checks if the array is filled: if so, the array size is doubled.
	//---------------------------------------------------------------------------------------
	void Resize()
	{
		// If the next empty space is the same as the size of the array, the next empty space is outside the array, and therefore, the array is full.
		if (mArraySize == mCurrentData)
		{
			// Create the new array that is double the size of the original array.
			T* biggerDataPtr = new T[mArraySize * 2];

			// Copies the data from the original array to the new bigger array.
			memcpy_s(biggerDataPtr, sizeof(T) * (mArraySize * 2), mDataPtr, sizeof(T) * mArraySize);

			// Delete the old array.
			delete[] mDataPtr;

			// Assign the new array to the pointer of the old array.
			mDataPtr = biggerDataPtr;

			// Update the size of the array to be double.
			mArraySize *= 2;
		}
	}

	// Pointer to the data in the array.
	T* mDataPtr;
	
	// Size of the array.
	int mArraySize;

	// The next empty space in the array.
	int mCurrentData;
};