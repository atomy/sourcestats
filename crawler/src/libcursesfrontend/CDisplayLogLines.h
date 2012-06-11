#ifndef DISPLAY_LOGLINES_H
#define DISPLAY_LOGLINES_H

#include "CDisplayLogLine.h"
#include <deque>

class CDisplayLogLines
{
public:
	CDisplayLogLines() {
		pthread_mutex_init(&muLogEntries, NULL);
	};

	CDisplayLogLines(CDisplayLogLines* pLines) {
		pthread_mutex_init(&muLogEntries, NULL);

		deque<CDisplayLogLine*> lines = pLines->getLinesNonThreadSave();
		deque<CDisplayLogLine*>::iterator it = lines.begin();

		while(it < lines.end()) {
			CDisplayLogLine* pLine = (*it);
			m_lines.push_back(pLine->clone());
			it++;
		}
	};

	~CDisplayLogLines() {
		deque<CDisplayLogLine*> entriesCopy;
		deque<CDisplayLogLine*>::iterator it = m_lines.begin();
		while(it < m_lines.end()) {
			CDisplayLogLine* pLine = (*it);
			delete(pLine);
			it++;
		}
	};

	deque<CDisplayLogLine*> getDuplicateLogEntries() {
		pthread_mutex_lock(&muLogEntries);  // LOCK
		deque<CDisplayLogLine*> entriesCopy;
		deque<CDisplayLogLine*>::iterator it = m_lines.begin();
		while(it < m_lines.end()) {
			CDisplayLogLine* pLine = (*it);
			entriesCopy.push_back(pLine->clone());
			it++;
		}
		pthread_mutex_unlock(&muLogEntries);  // UNLOCK

		return entriesCopy;
	}

	CDisplayLogLines* clone() {
		CDisplayLogLines* pNew = NULL;

		pthread_mutex_lock(&muLogEntries);		// LOCK
		pNew = new CDisplayLogLines(this);
		pthread_mutex_unlock(&muLogEntries);  // UNLOCK

		return pNew;
	}

	void push_front_non_threadsafe( CDisplayLogLine* pLine ) {
		m_lines.push_front(pLine);
	}

	int size_non_threadsafe() {
		return m_lines.size();
	}

	void pop_back_non_threadsafe() {
		m_lines.pop_back();
	}

	void lock() {
		pthread_mutex_lock(&muLogEntries);		// LOCK
	}

	void unlock() {
		pthread_mutex_unlock(&muLogEntries);	// UNLOCK
	}

	// not thread safe!
	deque<CDisplayLogLine*>& getLinesNonThreadSave() {
		return m_lines;
	}

private:
	pthread_mutex_t muLogEntries;
	deque<CDisplayLogLine*> m_lines;
};

#endif // DISPLAY_LOGLINES_H