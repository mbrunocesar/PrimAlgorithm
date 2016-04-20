#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//
// Nome: Bruno Cesar de Lima de Paula
// NUSP: 8516351
//


// When in the file mode we read and write to file, elsewhere we use the system input and output
bool flagReadFromFile = true;

// When in debug mode we print the arrays and matrixes for better understanding of execution
bool flagDebugMode = false;


int numVertex;
int numEdges;
int numFinalEdges = -1;


float **graphMatrix;
float totalCost;

float maximumCost = 65535;


// working with maximum 1000 vertex and 80 lines words
char readLines[1000][80];  
char *pch;


// Debug Auxiliar Functions
void printIntArray(int *intArray, int arraySize) {
	if (flagDebugMode == true) {
		int counter = 0;

		printf("------------\n");
		while (counter < arraySize) {
			printf("+ %d ", intArray[counter]);
			counter++;
		}
		printf("\n");
	}
}

void printFloatArray(float *floatArray, int arraySize) {
	if (flagDebugMode == true) {
		int counter = 0;

		printf("------------\n");
		while (counter < arraySize) {
			printf("+ %f ", floatArray[counter]);
			counter++;
		}
		printf("\n");
	}
}

void printFloatMatrix(float **floatMatrix, int arraySize) {
	if (flagDebugMode == true) {
		int counter = 0;

		printf("++++++++++++\n");
		while (counter < arraySize) {
			printFloatArray(floatMatrix[counter], arraySize);
			counter++;
		}
		printf("\n++++++++++++\n");
	}
}

// From a string input like 4 5 or 3 6 7 gets the first N integers
int* getIntegersFromSpaceSeparatedString(char *line, int numbersToRead) {
	static int values[4] = {0};
	int counter = 0;

	pch = strtok (line," ,-");
	while (pch != NULL && counter < numbersToRead) {
		values[counter] = atoi(pch);

		counter++;
		if (counter < numbersToRead) {
			pch = strtok (NULL, " ,-");
		}
	}
	return values;
}

// From a string input pre-processed like 3.6 removes any impurity and get it a float
float getFloatFromSpaceSeparatedString(char *line) {
	float values = 0.0;

	pch = strtok (NULL, " ,-");
	if (pch != NULL) {
		values = atof(pch);
	}
	return values;
}


// Gets the next line from a file or system input and save it to the current line
// It stop when found a null line or when reach the the max number of edges 
bool bufferizeNextLine (FILE *fr, int currentLine, int numEdges) {
	char readLine[80];
	if (flagReadFromFile == true) {
		fgets(readLine, 80, fr);
	} else {
		scanf("%s", readLine);
	}

	strcpy(readLines[currentLine], readLine);

	return currentLine < numEdges && readLine != NULL;
}


// Populate the graph in a float matrix structure
// where G[1][3] = 4.5 represents that the graph have
// a edge leaving node 1 and reaching node 3 with cost 4.5 
void populateGraphMatrix(char *arquivoEntrada) {

	char *line;
	FILE *fr = fopen(arquivoEntrada, "r");

	int *graphSizes;

	int numLines = 0;
	int currentLine = 0;

	int i,j;

	if (bufferizeNextLine(fr, numLines, 512)){
		numLines++;
	}

	if (numLines > 0) {
		line = readLines[0];
		graphSizes = getIntegersFromSpaceSeparatedString(line, 2);
		
		numVertex = graphSizes[0];
		numEdges = graphSizes[1];

		graphMatrix = (float **) malloc(numVertex*sizeof(float *));
		
		for(i = 0 ; i<numVertex;i++){
			graphMatrix[i] = (float*) malloc (numVertex*sizeof(float));
			
			for (j = 0; j < numVertex; j++){
				// Initialize every edge with the higher possible value
				graphMatrix[i][j] = maximumCost;
			}
		}

		printIntArray(graphSizes, 8);
		currentLine++;
	}

	while (bufferizeNextLine(fr, numLines, numEdges)){
		numLines++;
	}
	fclose(fr);

	while (currentLine <= numLines){
		line = readLines[currentLine];
		graphSizes = getIntegersFromSpaceSeparatedString(line, 2);

		float bufferizedValue = getFloatFromSpaceSeparatedString(line);

		// Check every edge present in graph to see if is lower than the current value
		// Remembering that base value is the highest possible
		if (bufferizedValue < graphMatrix[graphSizes[0]][graphSizes[1]]) {
			graphMatrix[graphSizes[0]][graphSizes[1]] = bufferizedValue;
			// How the graph dont have a direction we add every edge with a from-to and to-from form
			graphMatrix[graphSizes[1]][graphSizes[0]] = bufferizedValue;
		}

		currentLine++;
	}

	printFloatMatrix(graphMatrix, numVertex);
}


// Prim algorithm implementation
void executePrimAlgorithm() {
	bool selectedVertex[numVertex];

	int counter = 1;
	selectedVertex[0] = true;
	while (counter < numVertex) {
		selectedVertex[counter] = false;
		counter++;
	}

	int numSelectedVertex = 0;

	int i, j;

	int edgeFrom;
	int edgeTo;
	float edgeCost;

	float lowestCost;
	totalCost = 0;

	int numTargetEdges = numVertex - 1; 

	bool hasUpdate = true;

	while (numSelectedVertex < numTargetEdges && hasUpdate) {
		lowestCost = maximumCost;
		hasUpdate = false;

		i = 0;
		while (i < numVertex) {
			if (selectedVertex[i]) {

				j = 0;
				while (j < numVertex) {
					if (!selectedVertex[j]) {
						if (graphMatrix[i][j] < lowestCost) {
							lowestCost = graphMatrix[i][j];
							edgeFrom = i;
							edgeTo = j;
							edgeCost = lowestCost;
						} 

					}

					j++;
				}
				
			}

			i++;
		}

		if (edgeCost < maximumCost) {
			totalCost = totalCost + edgeCost;
			selectedVertex[edgeTo] = true;
			hasUpdate = true;

			sprintf(readLines[numSelectedVertex], "%d %d %f\n", edgeFrom, edgeTo, edgeCost);

			if (flagDebugMode == true) {
				printf("de %d para %d com custo %f\n", edgeFrom, edgeTo, edgeCost);
			}

			numSelectedVertex++;
		}
	}
	numFinalEdges = numSelectedVertex;
	//printf("Com %d\n", numFinalEdges);
	//printf("Custo %f\n", totalCost);
}


// Save every line of the selected vertex and his size to a file
void saveGraphToFile(char *arquivoSaida) {
	if (flagReadFromFile == true) {
		FILE *fw = fopen(arquivoSaida, "ab+");

		fprintf(fw, "%f\n", totalCost);
		int counter = 0;
		while (counter < numFinalEdges) {
			fprintf(fw, "%s", readLines[counter]);
			counter++;
		}

		fclose(fw);
	}
}


// Destroy all malloc allocated resources
void destroyGraph() {
	int i;
	for(i = 0 ; i<numVertex;i++){
		free(graphMatrix[i]);
	}
	free(graphMatrix);
}


// The magic starts here!
int main(int numParams, char *params[]){
	char *arquivoEntrada = params[1];
	char *arquivoSaida = params[2];

	populateGraphMatrix(arquivoEntrada);

	executePrimAlgorithm();
	saveGraphToFile(arquivoSaida);

	destroyGraph();

	return 0;
}
