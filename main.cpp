#include "stats.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <stdexcept>

#define DATA_BUFFER_CHUNK_MULTIPLICATOR 10

enum measure_of_location {
	COMPUTE_MEAN = 0,
	COMPUTE_MEDIAN, 
	COMPUTE_MIN,
	COMPUTE_MAX,
	COMPUTE_VARIANCE,
	COMPUTE_STD_DEVIATION,
	COMPUTE_END,
};

static int ignore_last_flag = 0;
static int ignore_first_flag = 0;
static int gentle_flag = 0;
static char *delim = NULL;
static int commands_queue_counter;
static int commands_queue[COMPUTE_END];

static void parse_options(int argc, char **argv)
{
	int opt, option_index;
	
	static const struct option long_options[] = {
		{"ignore-last", no_argument, &ignore_last_flag, 1},
		{"ignore-first", no_argument, &ignore_first_flag, 1},
		{"gentle", no_argument, &gentle_flag, 1},
		{"mean", no_argument, 0, 'm'},
		{"median", no_argument, 0, 'x'},
		{"variance", no_argument, 0, 'v'},
		{"std_deviation", no_argument, 0, 's'},
		{"min", no_argument, 0, 'l'},
		{"max", no_argument, 0, 'o'},
		{"delim", required_argument, 0, 'd'},
		{0, 0, 0, 0},
	};

	commands_queue_counter = 0;

	while(1) {
		/* read parameter */
		opt = getopt_long(argc, argv, "mxlod:", long_options, &option_index);

		if (opt == -1)
			break;

		switch(opt) {
			case 0:
				/* Options sets a flag */
				break;
			case 'm':
				commands_queue[commands_queue_counter++] = COMPUTE_MEAN;
				break;
			case 'x':
				commands_queue[commands_queue_counter++] = COMPUTE_MEDIAN;
				break;
			case 'l':
				commands_queue[commands_queue_counter++] = COMPUTE_MIN;
				break;
			case 'o':
				commands_queue[commands_queue_counter++] = COMPUTE_MAX;
				break;
			case 'v':
				commands_queue[commands_queue_counter++] = COMPUTE_VARIANCE;
				break;
			case 's':
				commands_queue[commands_queue_counter++] = COMPUTE_STD_DEVIATION;
				break;
			case 'd':
				delim = (char *) malloc(strlen(optarg));
				strcpy(delim, optarg);
				break;
			case '?':
				exit(1);
				break;
			default:
				std::cout << "error parsing option: " << std::endl;
				exit(1);
		}

		if (!(commands_queue_counter <= COMPUTE_END)) {
			std::cout << "I only support " << COMPUTE_END << " different " 
				"calculations. Computing them one of them more "
				"than once doesn't make sense." << std::endl;
			exit(1);
		}
	}
}

static int double_compare(const void *item1, const void *item2)
{
	double *p1 = (double *) item1;
	double *p2 = (double *) item2;

	return (*p1 > *p2);
}

int main(int argc, char **argv)
{
	double *data;
	double tmp;
	int data_length = 0;
	int data_available = 0;

	char *word;
	char *line = NULL;
	size_t number = 0;
	int start = 0;
	int commands_queue_tmp;


	parse_options(argc, argv);

	if (delim == NULL) {
		delim = (char *) malloc(3);
		strcpy(delim, " ,;");	
	}

	data = (double *) xmalloc(DATA_BUFFER_CHUNK_MULTIPLICATOR*sizeof(double));
	data_available = DATA_BUFFER_CHUNK_MULTIPLICATOR;

	/* read data from stdin */
	while (getline(&line,&number,stdin) > 0) {
		// split in up with delim
		word = strtok(line, delim);

		while (word != NULL) {
			if (!(data_available - data_length)) {
				data = (double *) xrealloc(data, ((sizeof(double)*(data_available*DATA_BUFFER_CHUNK_MULTIPLICATOR))));
				data_available += DATA_BUFFER_CHUNK_MULTIPLICATOR;
			}
			try {
				tmp = std::stod(word, NULL);
				data[data_length++] = tmp;
			}
			catch (const std::invalid_argument &ia) {
				std::cout << "error: cannot convert " << word << " to a number" << std::endl;
				if (!gentle_flag)
					exit(1);
			}
			catch (const std::out_of_range &oor) {
				std::cout << "error: out of range for " << word << std::endl;
				if (!gentle_flag)
					exit(1);
			}
			
			word = strtok(NULL, delim);
		}
	}



	/* ok we collected everything, start computing */
	if (ignore_last_flag == 1)
		data_length--;
	
	if (ignore_first_flag == 1)
		start++;

	/* sort data, needed for median */
	qsort(data, data_length, sizeof(double), double_compare);

	commands_queue_tmp = 0;

	while(commands_queue_tmp < commands_queue_counter) {
		switch(commands_queue[commands_queue_tmp]) {
			case COMPUTE_MEAN:
				std::cout << compute_mean(data, start, data_length) << std::endl;
				break;
			case COMPUTE_MEDIAN:
				std::cout << compute_median(data, start, data_length) << std::endl;
				break;
			case COMPUTE_MIN:
				std::cout << data[start] << std::endl;
				break;
			case COMPUTE_MAX:
				std::cout << data[data_length-1] << std::endl;
				break;
			case COMPUTE_VARIANCE:
				std::cout << compute_variance(data, start, data_length) << std::endl;
				break;
			case COMPUTE_STD_DEVIATION:
				std::cout << compute_standard_deviation(data, start, data_length) << std::endl;
				break;
		}

		commands_queue_tmp++;	
	}
	
	free(delim);
	free(data);
	
	return 0;
}
