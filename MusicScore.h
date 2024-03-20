#pragma once

//#include <ScriptBasedGE.hpp>

class MusicScore {
public:
	int64 start = 0;
	int bpm = 60;
	std::map<unsigned int, unsigned int> notes;

	MusicScore() = default;

	MusicScore(unsigned long long start, int bpm)
		: start(start)
		, bpm(bpm) {

	}

	unsigned int get(int key) {
		unsigned int value = 0;
		auto it = notes.find(key);
		if (it != notes.end())
			value = it->second;
		return value;
	}

	void set(int key, unsigned int value) {
		if (value)
			notes[key] |= value;
		else
			notes.erase(key);
	}

	void unset(int key, unsigned int value) {
		if (value)
			notes[key] &= (~value);
	}

	void clear(int key) {
		notes[key] = 0;
	}

	bool Write(s3d::FilePath file) {
		BinaryWriter bw(file);

		if (!bw)
			return false;

		bw.write(&start, sizeof(start));
		bw.write(&bpm, sizeof(bpm));

		for (auto& n : notes) {
			bw.write(&n, sizeof(n));
		}

		return true;
	}

	bool Read(s3d::FilePath file) {
		BinaryReader br(file);

		if (!br) {
			start = 0;
			bpm = 60;
			notes.clear();
			return false;
		}

		br.read(&start, sizeof(start));
		br.read(&bpm, sizeof(bpm));

		std::pair<unsigned int, unsigned int> n;
		notes.clear();
		while (br.read(&n, sizeof(n))) {
			notes.insert(n);
		}

		return true;
	}
};
