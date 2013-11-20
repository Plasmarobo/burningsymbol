#ifndef CHAPTER_H_
#define CHAPTER_H_

struct Chapter;

class Chapter{
protected:
	std::string m_start; //Intro
	std::string m_map; //The map to load (Associated map)
	std::string m_title; //The title
	std::string m_description; //A short descriptor
};

#endif