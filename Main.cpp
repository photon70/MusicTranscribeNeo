# include <Siv3D.hpp> // Siv3D v0.6.14

#include "MusicScore.h"
#include "Beat.hpp"
#include "BitFlag.hpp"

enum class State {
	Default,
	DragWave,
	DragNote,
	Bpm,
	Edit,
};

void Main()
{
	//auto a = InnerProperty();
	//SaveFile::Load();
	//Settings::Apply();

	//RegisterYield(Script::GetEngine());

	const auto WindowSize = Size(1280, 720);

	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(WindowSize);
	Window::ResizeVirtual(WindowSize);

	// 背景の色を設定 | Set background color
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	Font font(20, Typeface::Bold);

	auto saveFile = JSON::Load(U"save_file.json");
	if (not saveFile) {
		saveFile = JSON();
	}
	String file = saveFile[U"folder"].getOr<String>(U"music/bgm1");

	Audio audio(file + U".mp3");
	MusicScore score;

	State state = State::Default;
	int scale = 1;
	int noteScale = 128;

	Array<Vec2> guiRegion, guiRegionL;
	for (int i : step(20)) {
		guiRegion << Vec2(960, 50 * i + 5);
		guiRegionL << Vec2(5, 50 * i + 5);
	}
	LineString lines(1080, Vec2{ 0, 150 });

	double start = 0, last = 0;
	double count = 0;
	double speed = 1.0;


	audio.setVolume(0.25);
	score.Read(file + U".dat");

	//MusicNotesCounter counter(score, audio);
	BeatCounter counter(static_cast<double>(score.start) / static_cast<double>(audio.sampleRate()), score.bpm);

	TextEditState textEdit;

	const Audio piano[24] = {{GMInstrument::Piano1, PianoKey::C3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::CS3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::D3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::DS3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::E3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::F3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::FS3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::G3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::GS3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::A3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::AS3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::B3, 0.1s },
							{ GMInstrument::Piano1, PianoKey::C4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::CS4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::D4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::DS4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::E4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::F4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::FS4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::G4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::GS4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::A4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::AS4, 0.1s },
							{ GMInstrument::Piano1, PianoKey::B4, 0.1s } };
	const Audio buzzer = { GMInstrument::Flute, PianoKey::C6, 0.05s };

	auto seekSamples = [&](int64 s) {
		audio.seekSamples(std::min(std::max(s, static_cast<int64>(0)), static_cast<int64>(audio.samples() - 1)));
		};

	while (System::Update())
	{
		int64 period = audio.sampleRate() * 60 / std::max(1, score.bpm);

		counter.update(audio.posSec());

		const auto currentNg8 = counter.getCurrentNg8().rounded(2);

		if (counter.isUpdated()) {
			Scene::Rect().drawFrame(10, Palette::Blue);
		}

		Rect(99, 49, 1082, 202).draw(Color(60, 60, 60)).drawFrame();
		Rect(99, 250, 1082, 202).draw(Color(60, 60, 60)).drawFrame();

		if (Rect(99, 49, 1082, 403).contains(Cursor::Pos())) {
			scale += static_cast<int>(Mouse::Wheel());
			scale = std::max(1, scale);
			if (MouseL.down()) {
				audio.pause();
				state = State::DragWave;
				//audio.seekSamples(std::min(std::max(audio.posSample() + static_cast<int64>(s3d::Mouse::Wheel()), static_cast<int64>(0)), static_cast<int64>(audio.samples() - 1)));
			}
		}

		Line(128, 0, 128, 720).draw(Palette::Black);

		{
			auto samplesL = audio.getSamples(0);

			if (samplesL != nullptr) {
				double avr = 0;
				for (int i = 0; i < lines.size(); ++i) {//for (int i = std::max(static_cast<int64>(28), audio.posSample()); i < std::min(audio.samples() - audio.posSample() - 1, lines.size()); ++i) {
					avr = 0;
					for (int k = 0; k < scale; ++k) {
						avr += samplesL[std::min(std::max(audio.posSample() + (i - 28) * scale, static_cast<int64>(0)), static_cast<int64>(audio.samples() - 1))] / static_cast<double>(scale);
					}
					lines[i].set((100.0 + i), (150.0 - avr * 100.0));
				}
				
				lines.draw(2, Palette::Orange);
			}
		}
		{
			auto samplesR = audio.getSamples(1);

			if (samplesR != nullptr) {
				double avr = 0;
				for (int i = 0; i < lines.size(); ++i) {//for (int i = std::max(static_cast<int64>(28), audio.posSample()); i < std::min(audio.samples() - audio.posSample() - 1, lines.size()); ++i) {
					avr = 0;
					for (int k = 0; k < scale; ++k) {
						avr += samplesR[std::min(std::max(audio.posSample() + (i - 28) * scale, static_cast<int64>(0)), static_cast<int64>(audio.samples() - 1))] / static_cast<double>(scale);
					}
					lines[i].set((100.0 + i), (351.0 - avr * 100.0));
				}

				lines.draw(2, Palette::Orange);
			}
		}

		for (auto i = ((audio.posSample() - 28 * scale - score.start) / period) * period + score.start; (i - audio.posSample()) / scale + 28 < 1080; i += period) {
			if (i < audio.posSample() - 28 * scale)
				continue;
			auto buf = (i - audio.posSample()) / scale + 28;
			Line(100 + buf, 50, 100 + buf, 450).draw(Palette::White);
		}

		if (Rect(0, 460, 1280, 260).contains(Cursor::Pos()) && MouseL.down())
			state = State::DragNote;

		unsigned int value = 0;

		if (state != State::Edit) {
			if (KeyDelete.down()) {
				if (score.get(currentNg8) != 0)
					buzzer.playOneShot();
				score.clear(currentNg8);
			}
			if (counter.isUpdated(2) && KeyA.pressed() || KeyA.down())
				value |= Bit(0);
			if (counter.isUpdated(2) && KeyS.pressed() || KeyS.down())
				value |= Bit(1);
			if (counter.isUpdated(2) && KeyD.pressed() || KeyD.down())
				value |= Bit(2);
			if (counter.isUpdated(2) && KeyF.pressed() || KeyF.down())
				value |= Bit(3);
			if (counter.isUpdated(2) && KeyG.pressed() || KeyG.down())
				value |= Bit(4);
			if (counter.isUpdated(2) && KeyH.pressed() || KeyH.down())
				value |= Bit(5);
			if (counter.isUpdated(2) && KeyJ.pressed() || KeyJ.down())
				value |= Bit(6);
			if (counter.isUpdated(2) && KeyK.pressed() || KeyK.down())
				value |= Bit(7);
			if (counter.isUpdated(2) && KeyL.pressed() || KeyL.down())
				value |= Bit(8);
			if (counter.isUpdated(2) && KeySemicolon_JIS.pressed() || KeySemicolon_JIS.down())
				value |= Bit(9);
			if (counter.isUpdated(2) && KeyColon_JIS.pressed() || KeyColon_JIS.down())
				value |= Bit(10);
			if (counter.isUpdated(2) && KeyRBracket.pressed() || KeyRBracket.down())
				value |= Bit(11);
			if (counter.isUpdated(2) && KeyQ.pressed() || KeyQ.down())
				value |= Bit(12);
			if (counter.isUpdated(2) && KeyW.pressed() || KeyW.down())
				value |= Bit(13);
			if (counter.isUpdated(2) && KeyE.pressed() || KeyE.down())
				value |= Bit(14);
			if (counter.isUpdated(2) && KeyR.pressed() || KeyR.down())
				value |= Bit(15);
			if (counter.isUpdated(2) && KeyT.pressed() || KeyT.down())
				value |= Bit(16);
			if (counter.isUpdated(2) && KeyY.pressed() || KeyY.down())
				value |= Bit(17);
			if (counter.isUpdated(2) && KeyU.pressed() || KeyU.down())
				value |= Bit(18);
			if (counter.isUpdated(2) && KeyI.pressed() || KeyI.down())
				value |= Bit(19);
			if (counter.isUpdated(2) && KeyO.pressed() || KeyO.down())
				value |= Bit(20);
			if (counter.isUpdated(2) && KeyP.pressed() || KeyP.down())
				value |= Bit(21);
			if (counter.isUpdated(2) && KeyGraveAccent.pressed() || KeyGraveAccent.down())
				value |= Bit(22);
			if (counter.isUpdated(2) && KeyLBracket.pressed() || KeyLBracket.down())
				value |= Bit(23);
		}
		if (value)
			if (KeyLShift.pressed() || KeyRShift.pressed()) {
				if ((value & score.get(currentNg8)) != 0)
					buzzer.playOneShot();
				score.unset(currentNg8, value);
				value = 0;
			}
			else
				score.set(currentNg8, value);
		if (counter.isAfterUpdate(currentNg8)) {
			value = 0;
		}

		for (auto i = ((audio.posSample() - 128 * noteScale - score.start) / (period / 4)); (i * period / 4 + score.start - audio.posSample()) / noteScale + 128 < 1280; ++i) {
			auto buf = (i * period / 4 + score.start - audio.posSample()) / noteScale + 128;
			Line(buf, 480, buf, 720).draw(i % 4 == 0 ? Palette::Black : Palette::White);
			auto value = score.get(i * 64);
			if (i % 4 == 0)
				font(Format(i / 4)).drawAt(buf, 465, Palette::Black);

			for (int k = 0; k < 12; ++k) {
				if (BitIf(value, Bit(k + 12))) {
					Circle(buf + 10, 485 + 20 * k + 7, 8).draw(HSV(30 * k, 0.8, 0.4), HSV(30 * k, 0.4, 0.2));
				}
				if (BitIf(value, Bit(k))) {
					Circle(buf, 485 + 20 * k, 10).draw(HSV(30 * k, 0.8, 0.8), HSV(30 * k, 0.4, 0.4));
				}
			}

		}

		switch (state) {
		default:
		case State::Default:
			if (textEdit.active) {
				state = State::Edit;
				audio.pause();
				break;
			}
			if (KeySpace.down()) {
				if (audio.isPlaying()) {
					audio.pause();
				}
				else {
					counter.reset(audio.posSec());
					audio.play();
				}
			}
			if (!audio.isPlaying() && KeyZ.down()) {
				audio.seekSamples(0);
				counter.reset(0);
			}
			if (KeyRight.down()) {
				seekSamples(counter.ng8ToSec(currentNg8 + Ng8(1,0)) * audio.sampleRate());
				counter.reset(audio.posSec());
				audio.pause();
			}
			if (KeyLeft.down()) {
				if (currentNg8 < Ng8(1, 0)) {
					seekSamples(0);
				}
				else
					seekSamples(counter.ng8ToSec(currentNg8 - Ng8(1,0)) * audio.sampleRate());
				counter.reset(audio.posSec());
				audio.pause();
			}
			if (KeyB.down()) {
				if (!audio.isPlaying()) {
					audio.seekSamples(score.start);
					audio.play();
				}
				start = Scene::Time();
				last = Scene::Time();
				count = 0;
				state = State::Bpm;
			}
			break;
		case State::DragWave:
			seekSamples(audio.posSample() - static_cast<int64>(Cursor::Delta().x) * scale);
			counter.reset(audio.posSec());
			if (!MouseL.pressed())
				state = State::Default;
			break;
		case State::DragNote:
			seekSamples(audio.posSample() - static_cast<int64>(Cursor::Delta().x) * noteScale);
			counter.reset(audio.posSec());
			if (!MouseL.pressed())
				state = State::Default;
			break;
		case State::Bpm:
			if (KeyB.down()) {
				last = Scene::Time();
				++count;
				score.bpm = Math::Round(count / (last - start) * 60.0);
				counter.reset(audio.posSec());
			}
			if (KeySpace.pressed()) {
				state = State::Default;
				counter.setBpm(score.bpm);
				audio.pause();
			}
			break;
		case State::Edit:
			if (!textEdit.active)
				state = State::Default;
		}

		font(U"bpm : " + Format(score.bpm)).draw(140, 10, state == State::Bpm ? Palette::Red : Palette::Black);

		if (SimpleGUI::Button(U"SetStart", guiRegionL[0]))
		{
			score.start = audio.posSample();
			counter.setStart(static_cast<double>(score.start) / static_cast<double>(audio.sampleRate()));
			counter.reset(audio.posSec());
		}

		if (textEdit.active && KeyEnter.down())
		{
			auto splited = textEdit.text.split(' ');
			if (splited[0].compare(U"open") == 0) {
				audio.stop();
				if (file)
					score.Write(file + U".dat");
				file = splited[1];
				audio = Audio(file + U".mp3");
				if (audio) {
					double start = 0, last = 0;
					double count = 0;
					double speed = 1.0;

					audio.setVolume(0.25);

					score.Read(file + U".dat");
					counter.setStart(static_cast<double>(score.start) / static_cast<double>(audio.sampleRate()));
					counter.setBpm(score.bpm);
					counter.reset(audio.posSec());
				}
			}
			if (splited[0].compare(U"speed") == 0) {
				speed = Parse<double>(splited[1]);
				audio.setSpeed(speed);
			}
			if (splited[0].compare(U"bpm") == 0) {
				score.bpm = Parse<int>(splited[1]);	
				counter.setBpm(score.bpm);
			}
			
		}
		SimpleGUI::TextBox(textEdit, guiRegion[0]);

		//if (SimpleGUI::Button(U"フルスクリーン切り替え", guiRegion[1]))
		//{
		//	// 画面内のランダムな場所に座標を移動
		//	// Move the coordinates to a random position in the screen
		//	Settings::ChangeIsFullScreen(!Settings::getIsFullScreen());
		//	SaveFile::Save();
		//	Settings::Apply();
		//}

		if (SimpleGUI::Slider(U"speed: {:.3f}"_fmt(speed), speed, 0.0, 4.0, guiRegion[1], 140, 130))
		{
			audio.setSpeed(speed);
		}

		if (counter.isUpdated(2)) {
			if (KeyDelete.pressed()) {
				for (auto ng8 : counter.getAllCurrentNg8(2)) {
					if (score.get(ng8) != 0)
						buzzer.playOneShot();
					score.clear(ng8);
				}
			}
			value |= score.get(currentNg8);
		}

		for (int k = 0; k < 24; ++k) {
			if (BitIf(value, Bit(k))) {
				piano[k].playOneShot();
			}
		}
	}

	if (file)
		score.Write(file + U".dat");

	saveFile[U"folder"] = file;
	saveFile.save(U"save_file.json");

	//SaveFile::Save();
}

//void Main()
//{
//	// 背景の色を設定する | Set the background color
//	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
//
//	// 画像ファイルからテクスチャを作成する | Create a texture from an image file
//	const Texture texture{ U"example/windmill.png" };
//
//	// 絵文字からテクスチャを作成する | Create a texture from an emoji
//	const Texture emoji{ U"🦖"_emoji };
//
//	// 太文字のフォントを作成する | Create a bold font with MSDF method
//	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
//
//	// テキストに含まれる絵文字のためのフォントを作成し、font に追加する | Create a font for emojis in text and add it to font as a fallback
//	const Font emojiFont{ 48, Typeface::ColorEmoji };
//	font.addFallback(emojiFont);
//
//	// ボタンを押した回数 | Number of button presses
//	int32 count = 0;
//
//	// チェックボックスの状態 | Checkbox state
//	bool checked = false;
//
//	// プレイヤーの移動スピード | Player's movement speed
//	double speed = 200.0;
//
//	// プレイヤーの X 座標 | Player's X position
//	double playerPosX = 400;
//
//	// プレイヤーが右を向いているか | Whether player is facing right
//	bool isPlayerFacingRight = true;
//
//	while (System::Update())
//	{
//		// テクスチャを描く | Draw the texture
//		texture.draw(20, 20);
//
//		// テキストを描く | Draw text
//		font(U"Hello, Siv3D!🎮").draw(64, Vec2{ 20, 340 }, ColorF{ 0.2, 0.4, 0.8 });
//
//		// 指定した範囲内にテキストを描く | Draw text within a specified area
//		font(U"Siv3D (シブスリーディー) は、ゲームやアプリを楽しく簡単な C++ コードで開発できるフレームワークです。")
//			.draw(18, Rect{ 20, 430, 480, 200 }, Palette::Black);
//
//		// 長方形を描く | Draw a rectangle
//		Rect{ 540, 20, 80, 80 }.draw();
//
//		// 角丸長方形を描く | Draw a rounded rectangle
//		RoundRect{ 680, 20, 80, 200, 20 }.draw(ColorF{ 0.0, 0.4, 0.6 });
//
//		// 円を描く | Draw a circle
//		Circle{ 580, 180, 40 }.draw(Palette::Seagreen);
//
//		// 矢印を描く | Draw an arrow
//		Line{ 540, 330, 760, 260 }.drawArrow(8, SizeF{ 20, 20 }, ColorF{ 0.4 });
//
//		// 半透明の円を描く | Draw a semi-transparent circle
//		Circle{ Cursor::Pos(), 40 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
//
//		// ボタン | Button
//		if (SimpleGUI::Button(U"count: {}"_fmt(count), Vec2{ 520, 370 }, 120, (checked == false)))
//		{
//			// カウントを増やす | Increase the count
//			++count;
//		}
//
//		// チェックボックス | Checkbox
//		SimpleGUI::CheckBox(checked, U"Lock \U000F033E", Vec2{ 660, 370 }, 120);
//
//		// スライダー | Slider
//		SimpleGUI::Slider(U"speed: {:.1f}"_fmt(speed), speed, 100, 400, Vec2{ 520, 420 }, 140, 120);
//
//		// 左キーが押されていたら | If left key is pressed
//		if (KeyLeft.pressed())
//		{
//			// プレイヤーが左に移動する | Player moves left
//			playerPosX = Max((playerPosX - speed * Scene::DeltaTime()), 60.0);
//			isPlayerFacingRight = false;
//		}
//
//		// 右キーが押されていたら | If right key is pressed
//		if (KeyRight.pressed())
//		{
//			// プレイヤーが右に移動する | Player moves right
//			playerPosX = Min((playerPosX + speed * Scene::DeltaTime()), 740.0);
//			isPlayerFacingRight = true;
//		}
//
//		// プレイヤーを描く | Draw the player
//		emoji.scaled(0.75).mirrored(isPlayerFacingRight).drawAt(playerPosX, 540);
//	}
//}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
