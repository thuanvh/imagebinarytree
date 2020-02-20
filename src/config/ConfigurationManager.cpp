#include "ConfigurationManager.h"
#include "learning/ImageClass.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <learning/LibSVMAdapter.h>
#include <algo/bptsegment.h>
using namespace std;
ConfigurationManager* ConfigurationManager::Manager;
/**
 * @brief Free memory
 */
void ConfigurationManager::removeClassList() {
  //cout<<"remove classes list"<<endl;
  if (classList != NULL) {
    for (int i = 0; i < classList->size(); i++) {
      delete(*classList)[i];
    }
    delete classList;
  }
}
/**
 * @brief Create and get a global instance
 */
ConfigurationManager* ConfigurationManager::instance() {
  if (ConfigurationManager::Manager == NULL) {
    ConfigurationManager::Manager = new ConfigurationManager();
  }
  return ConfigurationManager::Manager;
}
/**
 * @brief Load class list from a configuration file
 */
void ConfigurationManager::loadClassList() {
  classList = ImageClass::loadImageClasses("classname.txt");
}
/**
 * @brief Destructor
 */
ConfigurationManager::~ConfigurationManager() {
  removeClassList();
}

/**
 * @brief Constructor
 */
ConfigurationManager::ConfigurationManager() {
  cout << "load config file" << endl;
  loadClassList();
  
}

string trim(string str);
/**
 * @brief Load configuration
 */
void ConfigurationManager::loadConfig(const char* filename)
{
  ifstream ifs;
  ifs.open(filename);
  this->BPTProperty=0;
  while(ifs.good()){
    string line;
    getline(ifs,line);
    // trim
    line=trim(line);
    if(line.empty())
      continue;
    if(line[0]=='#')
      continue;
    // get data variable=value
    cout<<line<<endl;
    
    int pos=line.find('=');
    string first=line.substr(0,pos);
    string second=line.substr(pos+1,line.length()-pos-1);

    first=trim(first);
    second=trim(second);

    
    
    if(first.compare("ClassNameFile")==0){
      this->ClassNameFile=second;
      //LearningFileName=learning.txt
    }else if(first.compare("LearningFileName")==0){
      this->LearningFileName=second;
    }else if(first.compare("LearningFolder")==0){
      this->LearningFolder=second;
    }else if(first.compare("LearningClass")==0){
      this->LearningClass=second;
    }else if(first.compare("TestFolder")==0){
      this->TestFolder=second;
    }else if(first.compare("TestClass")==0){
      this->TestClass=second;
    }else if(first.compare("ResultFolder")==0){
      this->ResultFolder=second;
    }else if(first.compare("ResultColor")==0){
      this->ResultColor=second;
    }else if(first.compare("TestMode")==0){
      //PRUN_EVOL_THRESHOLD; PRUN_EVOL_AUTO,  PRUN_EVOL_REGION_COUNT
      if(second.compare("BPT_CLASSIFY_AFTER_SEGMENTATION")==0)
        this->TestMode=BPT_CLASSIFY_AFTER_SEGMENTATION;
      else if(second.compare("BPT_SEGMENTATION_AFTER_CLASSIFIER")==0)
        this->TestMode=BPT_SEGMENTATION_AFTER_CLASSIFIER;
      else if(second.compare("TEXTONBOOST")==0)
        this->TestMode=TEXTONBOOST;       
    }else if(first.compare("BPTMode")==0){
      if(second.compare("REGIONNUMBER")==0)
        this->BPTMode=PRUN_EVOL_REGION_COUNT;
      else if(second.compare("EVOLVEMENT_NO_THRESHOLD")==0)
        this->BPTMode=PRUN_EVOL_AUTO;
      else if(second.compare("EVOLVEMENT_THRESHOLD")==0)
        this->BPTMode=PRUN_EVOL_THRESHOLD;
    } else if(first.compare("BPTRegionNumber")==0){
      this->BPTRegionNumber=atoi(second.c_str());
    } else if(first.compare("BPTEvolvementThreshold")==0){
      this->BPTEvolvementThreshold=atof(second.c_str());
    }else if(first.compare("Classifier")==0){
      if(second.compare("SVM")==0)
        this->Classifier=SVM;
      else if(second.compare("KNN")==0)
        this->Classifier=KNN;
      else if(second.compare("SIFTSEARCH")==0)
        this->Classifier=SIFTSEARCH;
    }else if(first.compare("SVMType")==0){
      if(second.compare("C_SVC")==0)
        this->SVMType=C_SVC;
      //else if(second.compare("KNN"))
        //this->SVMType=KNN;
        //enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR }; /* svm_type */
        //enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */
        
    }else if(first.compare("SVMKernelType")==0){
      if(second.compare("POLY")==0)
        this->SVMKernelType=POLY;
      else if(second.compare("LINEAR")==0)
        this->SVMKernelType=LINEAR;
      else if(second.compare("RBF")==0)
        this->SVMKernelType=RBF;
      else if(second.compare("SIGMOID")==0)
        this->SVMKernelType=SIGMOID;
    }else if(first.compare("SVMDegree")==0){
      this->SVMDegree=atoi(second.c_str());
    }else if(first.compare("SVMGamma")==0){
      this->SVMGamma=atof(second.c_str());      
    }else if(first.compare("SVMC")==0){
      this->SVMC=atoi(second.c_str());
    }else if(first.compare("KNNk")==0){
      this->KNNk=atoi(second.c_str());
    }else if(first.compare("SIFTAllPointFile")==0){
      this->SIFTAllPointFile=second;
    }else if(first.compare("SIFTThreshold")==0){
      this->SIFTThreshold=atof(second.c_str());
    }    
    else if(first.compare("UseColor")==0){
      this->UseColor=atoi(second.c_str());
    }else if(first.compare("UseColorLight")==0){
      this->UseColorLight=atoi(second.c_str());
    }else if(first.compare("UseTexture")==0){
      this->UseTexture=atoi(second.c_str());
    }else if(first.compare("UseGFD")==0){
      this->UseGFD=atoi(second.c_str());
    }else if(first.compare("UsePosition")==0){
      this->UsePosition=atoi(second.c_str());
    }else if(first.compare("UseSIFT")==0){
      this->UseSIFT=atoi(second.c_str());
    }else if(first.compare("SIFTDictFile")==0){
      this->SIFTDictFile=second;
    }else if(first.compare("ColorSize")==0){
      this->ColorSize=atoi(second.c_str());
    }else if(first.compare("TextureGraySize")==0){
      this->TextureGraySize=atoi(second.c_str());
      
    }else if(first.compare("GFDAngle")==0){
      this->GFDAngle=atoi(second.c_str());
    }else if(first.compare("GFDRadius")==0){
      this->GFDRadius=atoi(second.c_str());
    }else if(first.compare("BPTUseTextureWavelette")==0){
      if(second.compare("1")==0){
        this->BPTProperty |= PARA_TEXTUREWAVELET;
      }
    }else if(first.compare("BPTUseHomogeneity")==0){
      if(second.compare("1")==0){
        this->BPTProperty |= PARA_HOMOGENEITY;
      }
    }else if(first.compare("BPTUseMeanColor")==0){
      if(second.compare("1")==0){
        this->BPTProperty |= PARA_MEANCOLOR;
      }
    }else if(first.compare("BPTUseRegionSize")==0){
      if(second.compare("1")==0){
        this->BPTProperty |= PARA_REGIONSIZE;
      }
    }else if(first.compare("BPTUseMeanGradient")==0){
      if(second.compare("1")==0){
        this->BPTProperty |= PARA_MEANGRADIENT;
      }
    }else if(first.compare("BPTColorCovariance")==0){
      if(second.compare("1")==0){
        this->BPTProperty |= PARA_COLORCOVARIANCE;
      }
    }
    
  }
}

