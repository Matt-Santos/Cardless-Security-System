% Capstone ECG Tester
% Written by Matthew Santos

pkg load signal;
pkg load ltfat;
ECG_Functions;
clear;clc;close all;
set(0, "defaultlinelinewidth", 1.5);
graphics_toolkit("qt");

% Varriables
sampleRate = 500;   %[Hz]
TestRecords = 1:2;    %Test Records
%TestRecords = [1:3 5 8:9 11:12 14:18 22:23 25:32 42:43 45:46 49:53 58 60 62:64 66:69 71 75 78 79 81:83 86 88];

% Program Start
%-----------------------------------------
for Record = TestRecords
  [y,t] = ReadECG(Record,sampleRate);
  RawECG = Fractionalize(y(1,:),0.1);
  CleanECG(Record,:) = Preprocessing(RawECG,sampleRate);
  Features = FeatureExtraction(CleanECG(Record,:),sampleRate,1);
  [NumRecords,FMean,FStd] = LoadFeatureNorms('feature_norms',length(Features));
  Features = NormalizeFeatures(Features,1,FMean,FStd);
  Scores(Record,:) = TestClassifiers(Features',NumRecords,'classifiers');
  [Access(Record) Identity(Record) Margin(Record,:)] = AuthorizeCheck(Scores(Record,:));
end
%-----------------------------------------
% Program End


%Diagnosing
%-----------------------------------------
for Record = TestRecords
  figure(1);clf;plot(CleanECG(Record,:));axis([1 500]);title("CleanECG");
  figure(2);bar(Scores(Record,:));title("Scores");
  figure(3);bar(Margin(Record,:)*100);title("ScoreMargin");
  if Record == Identity(Record)
    printf("Correct Identification #%d \n",Identity(Record));
  else
    printf("Failed to Identify #%d \n",Identity(Record));
  end
  pause();
end




