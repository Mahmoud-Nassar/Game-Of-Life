//
// Created by mahmood on 12/25/2018.
//

#ifndef PART2_GOF_THREAD_H
#define PART2_GOF_THREAD_H
#include "Thread.hpp"
#include "Game.hpp"

class GOF_thread : Thread {
    Game *game;

public :
    /**
     * constructs a new GOF
     * @param m_thread_id - the id of the new GOF thread
     * @param game - the game that the GOF  describes
     */
    GOF_thread(uint m_thread_id, Game *game) : Thread(m_thread_id) {
        this->game = game;
    }
    /**
     * destructs a GOF thread
     */
    ~GOF_thread() = default;

    /**
     * the work function of the thread
     * pops a job from the queue and excutes the job
     */
    void thread_workload() override {
        while (1) {
            auto tile_start = std::chrono::system_clock::now();
            Job *job1 = game->pcQueue->pop();
            bool isLastGen = game->last_gen;

            // execution algorthim
            uint rows_num=game->lines_num;
            uint cols_num=game->cols_num;
            bool_mat* next=game->next;
            bool_mat* curr=game->curr;

            for (uint i = job1->begin;
            i < job1->begin + job1->rows_num; i++) {
                for (uint j = 0; j < cols_num; ++j) {
                    int dead_n = 0, alive_n = 0;
                    for (uint k = i - 1; k <= i + 1; ++k) {
                        for (uint w = j - 1; w <= j + 1; ++w) {
                            if (k != i || w != j) {
                                if (w < 0 || w >= cols_num || k < 0 ||
                                    k >= rows_num) {
                                    dead_n++;
                                } else if ((*(curr))[k][w] == 0) dead_n++;
                                else alive_n++;
                            }
                        }
                        if ((*curr)[i][j] == 0 && alive_n == 3) {
                            (*(next))[i][j] = 1;
                        } else if ((*curr)[i][j] == 1 &&
                                   (alive_n == 2 || alive_n == 3)) {
                            (*(next))[i][j] = 1;
                        } else { (*(next))[i][j] = 0; }
                    }

                }
            }
            auto tile_end = std::chrono::system_clock::now();
            pthread_mutex_lock(&game->mutex);
            game->m_tile_hist.push_back(
                    (float) std::chrono::duration_cast<std::chrono::microseconds>(
                            tile_end - tile_start).count());
            pthread_mutex_unlock(&game->mutex);
            pthread_mutex_lock(&game->mutex);
            game->waiting_threads++;
            if (game->waiting_threads == game->m_thread_num) {
                pthread_cond_signal(&game->cond);
            }
            pthread_mutex_unlock(&game->mutex);
            delete job1;
            if (isLastGen)break;
        }
    }

};
    


#endif //PART2_GOF_THREAD_H
