//
//  AdColonyPublic.h
//
//  Created by Lev Trubov on 2/9/10.
//  Copyright 2010 Jirbo, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//Should be self-explanatory: used to set a parameter to determine how much information
//is logged out to the console in the course of running the ad manager and related
//advertising.

extern NSString* AdColonyLoggingOn;
extern NSString* AdColonyLoggingOff;


//--------********--------********--------********--------********--------********--------********--------*******

//The AdColonyAdministratorDelegate protocol methods must be implemented in order to
//supply necessary configuration data about the app.
@protocol AdColonyAdministratorDelegate <NSObject>

//Should return the application id provided by the AdColony website.
-(NSString *)adColonyApplicationID;

//Should return a dictionary mapping unique integer keys to zone ids.
//Provides a list of all ad zones in use throughout the app.
//Keys should be NSNumber objects with integer values (called the slot number).
//Values should be NSString objects with zone id values provided by the AdColony website.
//Do not use the same key for multiple zones.
-(NSDictionary *)adColonyAdZoneNumberAssociation;


@optional
//Should return the application version, which is be a numerical string, like @"1.1"
-(NSString *)adColonyApplicationVersion;

//Should return approrpriate constant string to determine the amount of AdColony console logging.
-(NSString *)adColonyLoggingStatus;

//*
//Should return YES if the app uses mobclix for purposes other than advertising
//(or advertising outside AdColony), and thus, initializes mobclix
//otherwise, return no, so AdColony can initialize mobclix itself.
//If MCApplicationID parameter is set in the app's plist file, that id will be used
//Otherwise, AdColony will try to get the id from its servers
-(BOOL)adColonyMobclixInitializedExternally;//*/

//Is called when the video zone is turned off or server fails to return videos
-(void)noVideoFill;

-(void)adColonyVideoAdsReady;
-(void)adColonyNoVideoAdsReady;

//Should implement any app-specific code that should be run when AdColony has successfully rewarded
//virtual currency after a video. For example, contact a game server to determine the current total of 
//virtual currency after the award.
-(void)adColonyVirtualCurrencyAwarded;

//Should implement any app-specific code that should be run when AdColony has failed to reward virtual
//currency after a video. For example, update the user interface with the results of calling 
//virtualCurrencyAwardAvailable to disable or enable launching virtual currency videos.
-(void)adColonyVirtualCurrencyNotAwarded;

@end

//--------********--------********--------********--------********--------********--------********--------********

//Methods that should be implemented in classes that launch 
//ads that will take over the screen, either on appearance, or on click. 
@protocol AdColonyTakeoverAdDelegate <NSObject>

@optional
//Should implement any app-specific code that should be run when an ad that takes over the screen begins
//(for example, pausing a game if a video ad is being served in the middle of a session).
-(void)adColonyTakeoverBegan;

//Should implement any app-specific code that should be run when an ad that takes over the screen ends
//(for example, resuming a game if a video ad was served in the middle of a session).
-(void)adColonyTakeoverEnded:(BOOL)withVirtualCurrencyAward;

//Should implement any app-specific code that should be run when a banner ad appears on screen, such as view
//resizing
-(void)adColonyBannerAdServed;

//Should implement any app-specific code that should be run when a banner ad is called for and fails to
//appear on screen, such as view resizing
-(void)adColonyBannerAdNotServed;

//Should implement any app-specific code that should be run when AdColony is unable to play a video ad 
//or virtual currency video ad
-(void)adColonyVideoAdNotServed;



@end

//--------********--------********--------********--------********--------********--------********--------********

//This class contains methods used to initialize AdColony and display all the types of
//advertisements supported by AdColony. It should be initialized with an object that implements
//the AdColonyAdministratorDelegate protocol methods that supply necessary data about the app.
@interface AdColonyAdministratorPublic : NSObject {
	//delegate object
	//if not nil, the manager gets app-specific data it needs
	//from methods in the AdColonyAdministratorDelegate protocol
	id<AdColonyAdministratorDelegate> delegate; 
}

//This initializes AdColony's internal state, including readying video ads to be played.
//IMPORTANT: This method must be called before using any other methods in AdColonyAdministratorPublic.
+(void)initAdministratorWithDelegate:(id<AdColonyAdministratorDelegate>)del;

//Call to check if video ads are ready to be played.
//Returns YES if the video playlist has been filled and readied successfully.
+(BOOL)didVideoFinishedLoading;

//Call to play a video ad. 
//Note: This call should not be attempted right after initAdministratorWithDelegate:,
//		unless used in conjunction with didVideoFinishedLoading.
+(void)serveVideoAdForDelegate:(id<AdColonyTakeoverAdDelegate>)del;

//Call to check if it is possible to get a virtual currency reward for playing a video
//Returns NO if virtual currency hasn't been configured in the AdColony.com control panel
//Returns NO if the user's daily reward cap has been reached
//Returns YES otherwise
+(BOOL)virtualCurrencyAwardAvailable;

//Call to play a virtual currency video ad. 
//Note: This call should not be attempted immediately after initAdministratorWithDelegate:, 
//		unless used in conjunction with didVideoFinishedLoading.
//Note: Virtual currency must be enabled in the app's video zone on AdColony.com, or no video will play
//Note: The function virtualCurrencyAwardAvailable must return YES, or no video will play
+(void)serveVideoAdForVirtualCurrencyWithDelegate:(id<AdColonyTakeoverAdDelegate>)del andPostPopup:(BOOL)popup;

//Call this optional function to display a pre-video popup for virtual currency videos.
//Note: If using this popup, it is unnecessary to directly call serveVideoAdForVirtualCurrencyWithDelegate:,
//because the popup contains a button that will play a video ad when pressed.
+(void)displayVirtualCurrencyPrePopupWithDelegate:(id<AdColonyTakeoverAdDelegate>)del andPostPopup:(BOOL)popup;

//Returns the name of the virtual currency rewarded on each video play
//The returned value will match the value set in the the adcolony.com control panel
//Note: You must first initialize AdColony using initAdministratorWithDelegate: before using this function
+(NSString*)getVirtualCurrencyName;

//Returns the amount of virtual currency rewarded on each video play
//The returned value will match the value set in the the adcolony.com control panel
//Note: You must first initialize AdColony using initAdministratorWithDelegate: before using this function
+(int)getVirtualCurrencyRewardAmount;

//Turns off all ads including banners and videos. Can be used in the in-app purchase module
+(void)turnAllAdsOff;



@end


//--------********--------********--------********--------********--------********--------********--------********

//This class connects the banner ad system to displays of the stable banner ads
//If created in interface builder, set the AdColonyStableBannerAdViewPublic's tag property to
//associtate the banner with a slot number

@interface AdColonyStableBannerAdViewPublic : UIView {
	id<AdColonyTakeoverAdDelegate> delegate;
}

@property(nonatomic, assign) IBOutlet id<AdColonyTakeoverAdDelegate> delegate;

//Creates the view without a xib. Place its view in code using the 
//beginAdInView:atPosition method
-(id)initWithNoNibForSlot:(int)slotNumber;
-(id)initWithNoNibForSlot:(int)slotNumber forAdSize:(CGSize)adSize;
-(id)initWithNoNibForSlot:(int)slotNumber forAdSize:(CGSize)adSize andDelegate:(id<AdColonyTakeoverAdDelegate>)del;
-(id)initWithNoNibForZoneID:(NSString *)zoneID;
-(id)initWithNoNibForZoneID:(NSString *)zoneID forAdSize:(CGSize)adSize;
-(id)initWithNoNibForZoneID:(NSString *)zoneID forAdSize:(CGSize)adSize andDelegate:(id<AdColonyTakeoverAdDelegate>)del;

//Places the ad view into a view specified, with the ad centered at the specified
//point.
-(void)beginAdInView:(UIView *)view atPosition:(CGPoint)position;
-(void)beginAdInView:(UIView *)view atPosition:(CGPoint)position error:(NSError **)error;

//Refreshes ad. Use if ad is not configured to reload periodically, or to supplement the periodic
//reloads by reloads on user-supplied events
-(void)refreshCurrentAd;

@end
