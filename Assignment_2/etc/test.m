clear all;close all;clc;

[ori_file , Fs] = audioread('ahh.wav');
[output_file , Fs_1] = audioread('ahh_output.wav');
compare_file = zeros(size(ori_file));
delaySecs = 0.005;
gain = 0.99;
Delayline=zeros(floor(delaySecs*Fs),2);
for n=1:length(ori_file(:,1))
    compare_file(n,:)=ori_file(n,:)+gain.*Delayline(end,:);
    Delayline=[compare_file(n,:); Delayline(1:end-1,:)];
end
audiowrite('ahh_matlab_output.wav' , compare_file, Fs);
disp(sum(sum(abs(output_file-compare_file))))


[ori_file , Fs] = audioread('sweep.wav');
[output_file , Fs_1] = audioread('sweep_output.wav');
compare_file = zeros(size(ori_file));
delaySecs = 0.1;
gain = -0.5;
Delayline=zeros(floor(delaySecs*Fs),2);
for n=1:length(ori_file(:,1))
    compare_file(n,:)=ori_file(n,:)+gain.*Delayline(end,:);
    Delayline=[ori_file(n,:); Delayline(1:end-1,:)];
end
audiowrite('sweep_matlab_output.wav' , compare_file, Fs);
disp(sum(sum(abs(output_file-compare_file))))

