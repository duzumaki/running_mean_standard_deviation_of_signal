#include <sndfile.h>

#include <cmath>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

float mean(float *buffer, int sample_count) {
    float total = 0;
    for (int i = 0; i < sample_count; i++) {
        total = total + buffer[i];
    }
    return total / sample_count;
}

float standard_deviation(float *samples, int sample_count, float mean) {
    // A measure of how far away the samples deviate away from the mean
    float variance = 0;
    for (int i = 0; i < sample_count; i++) {
        variance = variance + pow((samples[i] - mean), 2);
    }
    variance = variance / sample_count;
    return sqrt(variance);
}

float running_mean(float *samples, int sample_count) {
    float mean = 0;
    float current_sample = 0;
    float sum = 0;

    for (int i = 0; i < sample_count; i++) {
        current_sample++;
        sum = sum + samples[i];
        mean = sum / current_sample;
    }

    return mean;
}

float running_variance(float *samples, int sample_count) {
    // memory effecient algorithm for getting the running variance of a signal.
    float running_sum = 0;
    float sum_squares = 0;
    float variance = 0;
    float current_sum_square_avg = 0;
    float current_sample = 0;

    for (int i = 0; i < sample_count; i++) {
        current_sample++;
        running_sum = running_sum + samples[i];
        sum_squares = sum_squares + pow(samples[i], 2);

        current_sum_square_avg = pow(running_sum, 2) / sample_count;
        variance = (sum_squares - current_sum_square_avg) / sample_count;
    }

    return sqrt(variance);
}

int main() {
    // create an SNDFILE object which respresents an audio file that
    // has been opened for reading
    const char *filename = "audio.wav";
    SF_INFO info;
    SNDFILE *file = sf_open(filename, SFM_READ, &info);

    // create a buffer for the samples to be inserted into
    // Sample count is derived from the sample rate per second of the file * the channel count
    const int samples = info.frames * info.channels;

    // file is read during run time so we have to allocate the buffer on the heap
    // as we can't know before hndle how many samples the file will have
    float *buffer = new float[samples];

    // we want to read all samples from the audio file into the buffer
    const int num_frames_read = sf_readf_float(file, buffer, info.frames);

    if (num_frames_read != info.frames) {
        std::cerr << "Failed to read wav data from " << filename;
        std::cerr << " Read " << num_frames_read << " frames, instad of " << info.frames << endl;
        sf_close(file);

        // since we allocated the buffer on the heap we have to release the memory here
        delete[] buffer;
        return 1;
    }
    float mean_of_samples = mean(buffer, samples);
    float sd = standard_deviation(buffer, samples, mean_of_samples);
    float running_avg = running_mean(buffer, samples);
    float running_sd = running_variance(buffer, samples);

    cout << running_avg << "\n";
    cout << running_sd << "\n";
    cout << mean_of_samples << "\n";
    cout << sd << endl;
}