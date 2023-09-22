//@file AudioSettings.h

#ifndef AUDIOSETTINGS_H_INCLUDED
#define AUDIOSETTINGS_H_INCLUDED

//音声再生プレイヤー番号
namespace AudioPlayer
{
	inline constexpr int bgm = 0;//BGMの再生に使うプレイヤー番号
	inline constexpr int bossbgm = 1;//BGMの再生に使うプレイヤー番号
}

//BGM設定
namespace BGM
{
	inline constexpr char title[]      = "Res/Audio/Opening.mp3";		//タイトル画面のBGM
	inline constexpr char load[]       = "Res/Audio/Loading.mp3";		//ロード画面のBGM
	inline constexpr char stageclear[] = "Res/Audio/StageClear.mp3";	//ゲームクリア画面のBGM
	inline constexpr char gameover[]   = "Res/Audio/GameOver.mp3";		//ゲームオーバー画面のBGM

	inline constexpr char stage01[]    = "Res/Audio/Stage.mp3";			//ステージのBGM
	inline constexpr char boss[]       = "Res/Audio/Boss.mp3";			//ステージのBGM


}

//効果音設定
namespace SE
{
	inline constexpr char playerShot[] = "Res/Audio/shoot.wav";			//弾の発射音
	inline constexpr char playerExplosion[] = "Res/Audio/BombL.wav";	//自機の爆発音
	inline constexpr char enemyExplosionS[] = "Res/Audio/ExpS.wav";		//爆発（小）
	inline constexpr char enemyExplosionM[] = "Res/Audio/ExpM.wav";		//爆発（中）
	inline constexpr char enemyExplosionL[] = "Res/Audio/ExpL.wav";		//爆発（大）
	inline constexpr char BossExplosion[]   = "Res/Audio/BossExp.wav";	//ボスの爆発音

	inline constexpr char Click[]		= "Res/Audio/Coin.wav";			//決定音
	inline constexpr char Click2[]      = "Res/Audio/Coin2.wav";		//決定音
	inline constexpr char powerup[]     = "Res/Audio/PowerUp.wav";		//アイテムの取得音
	inline constexpr char lasttrigger[] = "Res/Audio/LastTrigger.mp3";	//必殺技の音


}


#endif //AUDIOSETTINGS_H_INCLUDED