#include "Game.hpp"
#include "utils.hpp"
#include "GOF_thread.hpp"

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/

Game::Game(game_params par){
    this->m_gen_num=par.n_gen;
    this->m_thread_num=par.n_thread;
    this->filename=par.filename;
    this->m_threadpool=vector<Thread*>();
    this->interactive_on=par.interactive_on;
    this->print_on=par.print_on;
	this->field=utils::read_lines(this->filename);
	this->lines_num=(uint)this->field.size();
	this->cols_num=(uint)utils::split(field[0],' ').size();
	if (lines_num<m_thread_num) m_thread_num=lines_num;
	/******************* init mutex ***********/
	pthread_mutexattr_t Attr1;
	pthread_mutexattr_init(&Attr1);
	pthread_mutexattr_settype(&Attr1, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&mutex, &Attr1);
	pthread_cond_init(&cond, NULL);
	this->waiting_threads=0;
	this->last_gen=0;
}

void Game::run() {
	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		if (i==m_gen_num-1) last_gen=1;
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(NULL);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}


void Game::_init_game() {
	// Create game fields
	 curr=new bool_mat;
	 next=new bool_mat;
	for (uint i=0;i<this->lines_num;i++) {
		vector<bool> temp;
		for (uint j=0;j<this->cols_num*2;j+=2){
			char x=this->field[i][j]=='1';
			temp.push_back(x);
		}
		curr->push_back(temp);
	}
	*next=*curr;
	this->pcQueue=new PCQueue<Job*>;

	// Create threads
	// Start the threads
	for (uint i=0;i<m_thread_num;i++){
		GOF_thread* thread=new GOF_thread(i,this);
		m_threadpool.push_back((Thread*)thread);
		m_threadpool[i]->start();
	}
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
	uint tile_hight=this->lines_num/m_thread_num;
	uint add_to_last_tile_hight=lines_num%m_thread_num;
	uint last_tile_hight=tile_hight+add_to_last_tile_hight;
	for (uint i=0;i<m_thread_num-1;i++) {
		Job* job=new Job;
		job->rows_num=tile_hight;
		job->begin=i*tile_hight;
		this->pcQueue->push(job);
	}
	// iniate the last job
	Job* job=new Job;
	job->rows_num=last_tile_hight;
	job->begin=(m_thread_num-1)*tile_hight;
	this->pcQueue->push(job);
	// Wait for the workers to finish calculating
	pthread_mutex_lock(&mutex);
	while (waiting_threads<m_thread_num) {
		pthread_cond_wait(&cond, &mutex);
	}
	 pthread_mutex_unlock(&mutex);
	 waiting_threads=0;
	// Swap pointers between current and next field
     bool_mat* temp;
     temp=curr;
     curr=next;
     next=temp;
}

void Game::_destroy_game(){
	//Destroys board and frees all threads and resources
	delete curr;
	delete next;
	delete pcQueue;
	for (uint i=0;i<m_threadpool.size();i++) {
		m_threadpool[i]->join();
	    delete m_threadpool[i];
	}
	// Not implemented in the Game's destructor for testing purposes. 
	// Testing of your implementation will presume all threads are joined here

}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != NULL)
			cout << "<------------" << header << "------------>" << endl;
		
		// TODO: Print the board

		cout << u8"╔" << string(u8"═") * cols_num << u8"╗" << endl;
		for (uint i = 0; i < lines_num; ++i) {
			cout << u8"║";
			for (uint j = 0; j < cols_num; ++j) {
				cout << ((*curr)[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * cols_num << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}
}

const vector<float> Game::gen_hist() const{
	return this->m_gen_hist;
}

const vector<float> Game::tile_hist() const{
	return this->m_tile_hist;
}

uint Game::thread_num() const {
	unsigned long lines_num=utils::read_lines(this->filename).size();
	if (this->m_thread_num < lines_num){
		return this->m_thread_num;
	}
	return (uint)lines_num;
}
