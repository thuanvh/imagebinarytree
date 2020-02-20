// This is a reimplementation copy of using libsvm

#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <vector>
#include "LibSVMAdapter.h"
#include <fstream>
#include <iostream>

using namespace std;
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

static char *line = NULL;
static int max_line_len;
/**
 * @brief output error
 */
void exit_input_error(int line_num) {
  fprintf(stderr, "Wrong input format at line %d\n", line_num);
  exit(1);
}
/**
 * @brief Used to display nothing 
 */
void print_null(const char *s) {}
/*static char* readline(FILE *input) {
  int len;

  if (fgets(line, max_line_len, input) == NULL)
    return NULL;

  while (strrchr(line, '\n') == NULL) {
    max_line_len *= 2;
    line = (char *) realloc(line, max_line_len);
    len = (int) strlen(line);

    if (fgets(line + len, max_line_len - len, input) == NULL)
      break;
  }

  return line;
}
*/
/**
 * @brief Training method
 */
void LibSVMAdapter::training() {
  char input_file_name[1024];
  char model_file_name[1024];
  const char *error_msg;
  //cross_validation = 0;
  loadProblem(input_file_name);
  error_msg = svm_check_parameter(&prob, &param);

  if (error_msg) {
    fprintf(stderr, "Error: %s\n", error_msg);
  }

  if (cross_validation) {
    crossValidation();
  } else {
    model = svm_train(&prob, &param);

    if (svm_save_model(model_file_name, model)) {
      fprintf(stderr, "can't save model to file %s\n", model_file_name);
    }

    svm_free_and_destroy_model(&model);
  }

  svm_destroy_param(&param);

  free(prob.y);
  free(prob.x);
  free(x_space);
  free(line);
}
/**
 * @brief classify the vector input
 * @param classes Classes output
 * 
 */
void LibSVMAdapter::classify() {
  //char input_file_name[1024];
  //char model_file_name[1024];
  const char *error_msg;
  //cross_validation = 0;
  //loadProblem(input_file_name);
  error_msg = svm_check_parameter(&prob, &param);
  svm_set_print_string_function(&print_null);
  if (error_msg) {
    fprintf(stderr, "Error: %s\n", error_msg);
  }

  if (cross_validation) {
    crossValidation();
  } else {
    model = svm_train(&prob, &param);
    
    //if (svm_save_model(model_file_name, model)) {
    //  fprintf(stderr, "can't save model to file %s\n", model_file_name);
    //}

    classIds.clear();

    for (int i = 0; i < testNodeList.size(); i++) {
      svm_node* node = testNodeList[i];
      double classname = svm_predict(model, node);
      classIds.push_back((int)classname);
      
      /*int j = 0;
      while (node[j].index != -1) {
        cout << node[j].value;
        cout << " ";
        j++;
      }
      cout << endl;
      */
    }

    svm_free_and_destroy_model(&model);
  }

  svm_destroy_param(&param);

  free(prob.y);
  free(prob.x);
  free(x_space);
  free(line);
}

/**
 * @brief Constructor
 */
LibSVMAdapter::LibSVMAdapter() {
  // default values
  param.svm_type = C_SVC;
  param.kernel_type = LINEAR;//RBF;
  param.degree = 3;
  param.gamma = 0;  // 1/num_features
  param.coef0 = 0;
  param.nu = 0.5;
  param.cache_size = 100;
  param.C = 1;
  param.eps = 1e-3;
  param.p = 0.1;
  param.shrinking = 1;
  param.probability = 0;
  param.nr_weight = 0;
  param.weight_label = NULL;
  param.weight = NULL;

  cross_validation = 0;
  isDistanceSupport =false;
}

/**
 * @brief Destructor
 */
LibSVMAdapter::~LibSVMAdapter()
{
  for(int i=0; i<testNodeList.size(); i++){
    svm_node* node = testNodeList[i];
    delete[] node;
  }
}

/**
 * @brief cross validation of LibSVM
 */
void LibSVMAdapter::crossValidation() {
  int i;
  int total_correct = 0;
  double total_error = 0;
  double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
  double *target = Malloc(double, prob.l);

  svm_cross_validation(&prob, &param, nr_fold, target);

  if (param.svm_type == EPSILON_SVR ||
      param.svm_type == NU_SVR) {
    for (i = 0;i < prob.l;i++) {
      double y = prob.y[i];
      double v = target[i];
      total_error += (v - y) * (v - y);
      sumv += v;
      sumy += y;
      sumvv += v * v;
      sumyy += y * y;
      sumvy += v * y;
    }

    printf("Cross Validation Mean squared error = %g\n", total_error / prob.l);

    printf("Cross Validation Squared correlation coefficient = %g\n",
        ((prob.l*sumvy - sumv*sumy)*(prob.l*sumvy - sumv*sumy)) /
        ((prob.l*sumvv - sumv*sumv)*(prob.l*sumyy - sumy*sumy))
          );
  } else {
    for (i = 0;i < prob.l;i++)
      if (target[i] == prob.y[i])
        ++total_correct;

    printf("Cross Validation Accuracy = %g%%\n", 100.0*total_correct / prob.l);
  }

  free(target);
};

/**
 * @brief read in a problem (in svmlight format)
 */
void LibSVMAdapter::loadProblem(const char *filename) {
  int elements, max_index, inst_max_index, i, j;
  ifstream ifFileLearn;
  ifFileLearn.open(filename);
  // lire l'entete de fichier


  //FILE *fp = fopen(filename, "r");
  char *endptr;
  char *idx, *val, *label;

  if (!ifFileLearn.good()) {
    fprintf(stderr, "can't open input file %s\n", filename);
    exit(1);
  }

  prob.l = 0;
  elements = 0;
  max_line_len = 1024;
  //line = Malloc(char, max_line_len);

  /// count the number of element and prob
  while (!ifFileLearn.eof()) {
    string firstline;
    getline(ifFileLearn, firstline);

    if (firstline == "")
      break;

    char c[firstline.length()];
    //memcpy(c,firstline.c_str());
    firstline.copy(c, firstline.length());
    char *p = strtok(c, " \t"); // label
    // features
    while (1) {
      p = strtok(NULL, " \t");
      if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
        break;
      ++elements;
    }
    ++elements;
    ++prob.l;
  }

  ifFileLearn.close();
  ifFileLearn.open(filename);
  //rewind(fp);
  prob.y = Malloc(double, prob.l);
  prob.x = Malloc(struct svm_node *, prob.l);
  x_space = Malloc(struct svm_node, elements);

  max_index = 0;
  j = 0;

  for (i = 0;i < prob.l;i++) {
    inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
    //readline(fp);
    string firstline;

    if (ifFileLearn.eof())
      break;

    getline(ifFileLearn, firstline);
    prob.x[i] = &x_space[j];
    char c[firstline.length()];
    firstline.copy(c, firstline.length());
    label = strtok(c, " \t\n");
    if (label == NULL) { // empty line
      //exit_input_error(i + 1);
      break;
    }
    prob.y[i] = strtod(label, &endptr);
    if (endptr == label || *endptr != '\0')
      exit_input_error(i + 1);
    while (1) {
      idx = strtok(NULL, ":");
      val = strtok(NULL, " \t");
      if (val == NULL)
        break;
      errno = 0;
      x_space[j].index = (int) strtol(idx, &endptr, 10);
      if (endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
        exit_input_error(i + 1);
      else
        inst_max_index = x_space[j].index;
      errno = 0;
      x_space[j].value = strtod(val, &endptr);
      if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
        exit_input_error(i + 1);
      ++j;
    }

    if (inst_max_index > max_index)
      max_index = inst_max_index;

    x_space[j++].index = -1;
  }

  if (param.gamma == 0 && max_index > 0)
    param.gamma = 1.0 / max_index;

  if (param.kernel_type == PRECOMPUTED)
    for (i = 0;i < prob.l;i++) {
      if (prob.x[i][0].index != 0) {
        fprintf(stderr, "Wrong input format: first column must be 0:sample_serial_number\n");
        exit(1);
      }

      if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index) {
        fprintf(stderr, "Wrong input format: sample_serial_number out of range\n");
        exit(1);
      }
    }
  // verifier the problem loading is correct
  /*
  cout<<"end of load problem"<<endl;
  for(int i=0; i<prob.l; i++){
    cout<<prob.y[i]<<" ";
    for(int j=0; j<elements; j++){
      cout<<prob.x[i][j].index<<":"<<prob.x[i][j].value<<" ";
      if(prob.x[i][j].index==-1)
        break;
        //getchar();
    }
    cout<<endl;
  }
  */

  ifFileLearn.close();
}
/**
 * @brief Get the test data
 */
void LibSVMAdapter::setTestData(const std::vector< std::vector< double >* >& testData) {
  int size1 = testData.size();

  for (int i = 0; i < size1; i++) {
    std::vector<double> vec = *testData[i];
    int size2 = vec.size();
    svm_node* nodeArray = Malloc(struct svm_node, size2+1);
    int count = 0;
    //nodeArray[0].index=nodeArray[0].value=0;
    for (int j = 0; j < size2; j++) {
      double val = (*testData[i])[j];

      if (val != 0) {
        nodeArray[count].index = j+1;
        nodeArray[count].value = val;
        count++;
      }
    }

    //if (count < size2) {
      nodeArray[count].index = -1;
      nodeArray[count].value = 0;
    //}
    /*
    /// validation of loading testing data
    for(int i=0; i<size2+1; i++){      
      cout<<nodeArray[i].index<<":"<<nodeArray[i].value<<" ";
      if(nodeArray[i].index==-1)
        break;
    }
    cout<<endl;
    //getchar();
    */
    
    testNodeList.push_back(nodeArray);
  }
}
/**
 * @brief Get the information string about the libsvm params
 */
string LibSVMAdapter::getInfo()
{
  string info("SVM ");
  switch(param.svm_type){
    case C_SVC:
      info +="C_SVC";
      break;
  }
  switch(param.kernel_type){
    case LINEAR:
      info +=" linear";
      break;
    case POLY:
      info+="poly degree=";
      info+=param.degree;
      break;
    case SIGMOID:
      info+="sigmoid sigmoid=";
      info+=param.gamma;
      break;
    case RBF:
      info += " RBF gama=";
      info += param.gamma;
      info += " c=";
      info += param.C;
      break;
  }
    
  return info;
}
/**
 * @brief reset all memory
 */
void LibSVMAdapter::reset()
{
  classIds.clear();
  classDistances.clear();
}
