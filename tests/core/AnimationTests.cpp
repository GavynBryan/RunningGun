#include <gtest/gtest.h>
#include <animation/AnimationClip.h>

// ============= AnimationClipData Tests =============

TEST(AnimationClipDataTest, DefaultConstruction) {
    AnimationClipData clip;
    
    EXPECT_TRUE(clip.Name.empty());
    EXPECT_FLOAT_EQ(clip.FrameSize.x, 0.0f);
    EXPECT_FLOAT_EQ(clip.FrameSize.y, 0.0f);
    EXPECT_EQ(clip.RowIndex, 0);
    EXPECT_EQ(clip.FrameCount, 0);
    EXPECT_FLOAT_EQ(clip.FrameDuration, 0.25f);
    EXPECT_FALSE(clip.Looping);
}

TEST(AnimationClipDataTest, CustomValues) {
    AnimationClipData clip;
    clip.Name = "walk";
    clip.FrameSize = Vec2(32.0f, 32.0f);
    clip.RowIndex = 2;
    clip.FrameCount = 8;
    clip.FrameDuration = 0.1f;
    clip.Looping = true;
    
    EXPECT_EQ(clip.Name, "walk");
    EXPECT_FLOAT_EQ(clip.FrameSize.x, 32.0f);
    EXPECT_EQ(clip.RowIndex, 2);
    EXPECT_EQ(clip.FrameCount, 8);
    EXPECT_FLOAT_EQ(clip.FrameDuration, 0.1f);
    EXPECT_TRUE(clip.Looping);
}

// ============= AnimationPlayback Tests =============

TEST(AnimationPlaybackTest, DefaultConstruction) {
    AnimationPlayback playback;
    
    EXPECT_EQ(playback.GetClip(), nullptr);
    EXPECT_EQ(playback.GetCurrentFrame(), 0);
    EXPECT_FALSE(playback.IsFinished());
    EXPECT_FALSE(playback.IsPlaying());
}

TEST(AnimationPlaybackTest, ConstructionWithClip) {
    AnimationClipData clip;
    clip.FrameCount = 4;
    clip.Looping = true;
    
    AnimationPlayback playback(&clip);
    
    EXPECT_EQ(playback.GetClip(), &clip);
    EXPECT_EQ(playback.GetCurrentFrame(), 0);
    EXPECT_FALSE(playback.IsFinished());
    EXPECT_TRUE(playback.IsPlaying());
}

TEST(AnimationPlaybackTest, SetClip) {
    AnimationClipData clip;
    clip.FrameCount = 4;
    clip.Looping = true;
    
    AnimationPlayback playback;
    playback.SetClip(&clip);
    
    EXPECT_EQ(playback.GetClip(), &clip);
    EXPECT_TRUE(playback.IsPlaying());
}

TEST(AnimationPlaybackTest, AdvanceFrames) {
    AnimationClipData clip;
    clip.FrameCount = 4;
    clip.FrameDuration = 0.1f;
    clip.Looping = true;
    
    AnimationPlayback playback(&clip);
    
    EXPECT_EQ(playback.GetCurrentFrame(), 0);
    
    // Advance past one frame duration
    playback.Advance(0.1f);
    EXPECT_EQ(playback.GetCurrentFrame(), 1);
    
    playback.Advance(0.1f);
    EXPECT_EQ(playback.GetCurrentFrame(), 2);
}

TEST(AnimationPlaybackTest, LoopingAnimation) {
    AnimationClipData clip;
    clip.FrameCount = 3;
    clip.FrameDuration = 0.1f;
    clip.Looping = true;
    
    AnimationPlayback playback(&clip);
    
    // Advance through all frames
    playback.Advance(0.1f); // Frame 1
    playback.Advance(0.1f); // Frame 2
    playback.Advance(0.1f); // Should wrap to frame 0
    
    EXPECT_EQ(playback.GetCurrentFrame(), 0);
    EXPECT_FALSE(playback.IsFinished());
    EXPECT_TRUE(playback.IsPlaying());
}

TEST(AnimationPlaybackTest, NonLoopingAnimation) {
    AnimationClipData clip;
    clip.FrameCount = 3;
    clip.FrameDuration = 0.1f;
    clip.Looping = false;
    
    AnimationPlayback playback(&clip);
    
    // Advance through all frames
    playback.Advance(0.1f); // Frame 1
    playback.Advance(0.1f); // Frame 2
    bool finished = playback.Advance(0.1f); // Should finish
    
    EXPECT_TRUE(finished);
    EXPECT_TRUE(playback.IsFinished());
    EXPECT_FALSE(playback.IsPlaying());
    EXPECT_EQ(playback.GetCurrentFrame(), 2); // Stays on last frame
}

TEST(AnimationPlaybackTest, Reset) {
    AnimationClipData clip;
    clip.FrameCount = 4;
    clip.FrameDuration = 0.1f;
    clip.Looping = false;
    
    AnimationPlayback playback(&clip);
    
    playback.Advance(0.3f); // Advance to frame 3
    playback.Advance(0.1f); // Finish
    
    EXPECT_TRUE(playback.IsFinished());
    
    playback.Reset();
    
    EXPECT_EQ(playback.GetCurrentFrame(), 0);
    EXPECT_FALSE(playback.IsFinished());
}

TEST(AnimationPlaybackTest, SingleFrameAnimation) {
    AnimationClipData clip;
    clip.FrameCount = 1;
    clip.FrameDuration = 0.1f;
    clip.Looping = false;
    
    AnimationPlayback playback(&clip);
    
    bool finished = playback.Advance(1.0f);
    
    EXPECT_TRUE(finished);
    EXPECT_TRUE(playback.IsFinished());
    EXPECT_EQ(playback.GetCurrentFrame(), 0);
}

TEST(AnimationPlaybackTest, SingleFrameLooping) {
    AnimationClipData clip;
    clip.FrameCount = 1;
    clip.FrameDuration = 0.1f;
    clip.Looping = true;
    
    AnimationPlayback playback(&clip);
    
    bool finished = playback.Advance(1.0f);
    
    EXPECT_FALSE(finished);
    EXPECT_FALSE(playback.IsFinished());
    EXPECT_TRUE(playback.IsPlaying());
}

TEST(AnimationPlaybackTest, AdvanceWithNoClip) {
    AnimationPlayback playback;
    
    bool result = playback.Advance(1.0f);
    
    EXPECT_FALSE(result);
}

TEST(AnimationPlaybackTest, AdvanceWhenFinished) {
    AnimationClipData clip;
    clip.FrameCount = 2;
    clip.FrameDuration = 0.1f;
    clip.Looping = false;
    
    AnimationPlayback playback(&clip);
    playback.Advance(0.2f); // Finish the animation
    
    EXPECT_TRUE(playback.IsFinished());
    
    // Further advances should do nothing
    bool result = playback.Advance(1.0f);
    EXPECT_FALSE(result);
}

TEST(AnimationPlaybackTest, LargeDeltaTimeSkipsFrames) {
    AnimationClipData clip;
    clip.FrameCount = 10;
    clip.FrameDuration = 0.1f;
    clip.Looping = true;
    
    AnimationPlayback playback(&clip);
    
    // Advance by 0.5 seconds (5 frames worth)
    playback.Advance(0.5f);
    
    EXPECT_EQ(playback.GetCurrentFrame(), 5);
}

TEST(AnimationPlaybackTest, GetNormalizedTime) {
    AnimationClipData clip;
    clip.FrameCount = 4;
    clip.FrameDuration = 0.25f; // 1 second total
    clip.Looping = false;
    
    AnimationPlayback playback(&clip);
    
    EXPECT_FLOAT_EQ(playback.GetNormalizedTime(), 0.0f);
    
    playback.Advance(0.25f); // 1/4 done
    // Frame 1, timer at 0 -> normalized = 1/4
    EXPECT_NEAR(playback.GetNormalizedTime(), 0.25f, 0.01f);
}
