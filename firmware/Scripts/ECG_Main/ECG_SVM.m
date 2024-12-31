% Capstone ECG SVM Generator
% Written by Matthew Santos

pkg load signal;
pkg load ltfat;
ECG_Functions;
clear;clc;close all;
set(0, "defaultlinelinewidth", 1.5);
graphics_toolkit("qt");

% Varriables
sampleRate = 500;     %[Hz]
RecordNumbers = [1 2]; %[1:3 5 8:9 11:12 14:18 22:23 25:32 42:43 45:46 49:53 58 60 62:64 66:69 71 75 78 79 81:83 86 88]; %RecordIndices

% Program Start
%-----------------------------------------
for Record=RecordNumbers
  [y,t] = ReadECG(Record,sampleRate);
  RawECG = Fractionalize(y(1,:),0.1);
  RawECG(Record,:) = Fractionalize(y(1,:),0.1);
  [CleanECG(Record,:) PPFilter] = Preprocessing(RawECG(Record,:),sampleRate);
  Features{Record} = FeatureExtraction(CleanECG(Record,:),sampleRate,0);
end
[FeatureMean,FeatureStd] = GetFeatureNormFactors(Features,RecordNumbers);
SaveFeatureNorms('feature_norms',length(RecordNumbers),FeatureMean,FeatureStd);
Features = NormalizeFeatures(Features,RecordNumbers,FeatureMean,FeatureStd);
Classifiers = GetClassifiers(Features,RecordNumbers);
SaveClassifiers('classifiers',Classifiers);
%-----------------------------------------
% Program End


%Diagnosing (Feature Plots)
%-----------------------------------------
figure(1);clf;PlotClassifier3D(2,7,9,Classifiers,Features);
Fscores = GetFscores(Features,RecordNumbers);
AvgFscores = mean(Fscores,1);
figure(2);bar(AvgFscores);title("Feature Fscores Based on 50 sample ECGs","fontsize",20);axis([0 28 26 max(AvgFscores)]);


