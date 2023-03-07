/* --------------------------------------------------
   Copyright (C): OpenGATE Collaboration
   This software is distributed under the terms
   of the GNU Lesser General  Public Licence (LGPL)
   See LICENSE.md for further details
   -------------------------------------------------- */

#include "../GateUniqueVolumeIDManager.h"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "GateDigiAttributeManager.h"

/* Macros to reduce the code size
   Use FILLFS when step is not used to avoid warning

    In the G4 docs:
    "The second argument of FillHits() method, i.e. G4TouchableHistory, is
   obsolete and not used. If user needs to define an artificial second geometry,
   use Parallel Geometries."
*/

#define FILLF [=](GateVDigiAttribute * att, G4Step * step)
#define FILLFS [=](GateVDigiAttribute * att, G4Step *)

void GateDigiAttributeManager::InitializeAllDigiAttributes() {

  // -----------------------------------------------------
  // Energy
  DefineDigiAttribute(
      "TotalEnergyDeposit", 'D',
      FILLF { att->FillDValue(step->GetTotalEnergyDeposit()); });
  DefineDigiAttribute(
      "PostKineticEnergy", 'D',
      FILLF { att->FillDValue(step->GetPostStepPoint()->GetKineticEnergy()); });
  DefineDigiAttribute(
      "PreKineticEnergy", 'D',
      FILLF { att->FillDValue(step->GetPreStepPoint()->GetKineticEnergy()); });
  DefineDigiAttribute(
      "KineticEnergy", 'D',
      // KineticEnergy is the same as PreKineticEnergy
      FILLF { att->FillDValue(step->GetPreStepPoint()->GetKineticEnergy()); });

  DefineDigiAttribute(
      "TrackVertexKineticEnergy", 'D',
      FILLF { att->FillDValue(step->GetTrack()->GetVertexKineticEnergy()); });

  DefineDigiAttribute(
      "EventKineticEnergy", 'D', FILLFS {
        const auto *event = G4RunManager::GetRunManager()->GetCurrentEvent();
        auto e = event->GetPrimaryVertex(0)->GetPrimary(0)->GetKineticEnergy();
        att->FillDValue(e);
      });

  // -----------------------------------------------------
  // Time
  DefineDigiAttribute(
      "LocalTime", 'D',
      FILLF { att->FillDValue(step->GetPostStepPoint()->GetLocalTime()); });
  DefineDigiAttribute(
      "GlobalTime", 'D',
      FILLF { att->FillDValue(step->GetPostStepPoint()->GetGlobalTime()); });
  DefineDigiAttribute(
      "TimeFromBeginOfEvent", 'D', FILLF {
        /*
         * GlobalTime = Time since the event in which the track belongs is
         * created
         *
         */
        const auto *event = G4RunManager::GetRunManager()->GetCurrentEvent();
        auto t = step->GetTrack()->GetGlobalTime() -
                 event->GetPrimaryVertex(0)->GetT0();
        att->FillDValue(t);
      });
  DefineDigiAttribute(
      "TrackProperTime", 'D', FILLF {
        auto t = step->GetTrack()->GetProperTime();
        att->FillDValue(t);
      });

  // -----------------------------------------------------
  // Misc
  DefineDigiAttribute(
      "Weight", 'D', FILLF { att->FillDValue(step->GetTrack()->GetWeight()); });
  DefineDigiAttribute(
      "TrackID", 'I',
      FILLF { att->FillIValue(step->GetTrack()->GetTrackID()); });
  DefineDigiAttribute(
      "ParentID", 'I',
      FILLF { att->FillIValue(step->GetTrack()->GetParentID()); });
  DefineDigiAttribute(
      "EventID", 'I', FILLFS {
        auto id =
            G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        att->FillIValue(id);
      });
  DefineDigiAttribute(
      "RunID", 'I', FILLFS {
        auto id = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
        att->FillIValue(id);
      });
  DefineDigiAttribute(
      "ThreadID", 'I',
      FILLFS { att->FillIValue(G4Threading::G4GetThreadId()); });
  DefineDigiAttribute(
      "TrackCreatorProcess", 'S', FILLF {
        const auto *p = step->GetTrack()->GetCreatorProcess();
        if (p != nullptr)
          att->FillSValue(p->GetProcessName());
        else
          att->FillSValue("none");
      });
  DefineDigiAttribute(
      "ProcessDefinedStep", 'S', FILLF {
        const auto *p = step->GetPreStepPoint()->GetProcessDefinedStep();
        if (p != nullptr)
          att->FillSValue(p->GetProcessName());
        else
          att->FillSValue("none");
      });
  DefineDigiAttribute(
      "ParticleName", 'S', FILLF {
        att->FillSValue(
            step->GetTrack()->GetParticleDefinition()->GetParticleName());
      });
  DefineDigiAttribute(
      "TrackVolumeName", 'S',
      FILLF { att->FillSValue(step->GetTrack()->GetVolume()->GetName()); });
  DefineDigiAttribute(
      "TrackVolumeCopyNo", 'I',
      FILLF { att->FillIValue(step->GetTrack()->GetVolume()->GetCopyNo()); });
  DefineDigiAttribute(
      "PreStepVolumeCopyNo", 'I', FILLF {
        const auto *touchable = step->GetPreStepPoint()->GetTouchable();
        auto depth = touchable->GetHistoryDepth();
        auto copyNb = touchable->GetVolume(depth)->GetCopyNo();
        att->FillIValue(copyNb);
      });
  DefineDigiAttribute(
      "PostStepVolumeCopyNo", 'I', FILLF {
        const auto *touchable = step->GetPostStepPoint()->GetTouchable();
        auto depth = touchable->GetHistoryDepth();
        auto copyNb = touchable->GetVolume(depth)->GetCopyNo();
        att->FillIValue(copyNb);
      });
  DefineDigiAttribute(
      "TrackVolumeInstanceID", 'I', FILLF {
        att->FillIValue(step->GetTrack()->GetVolume()->GetInstanceID());
      });
  DefineDigiAttribute(
      "PreStepUniqueVolumeID", 'U', FILLF {
        auto *m = GateUniqueVolumeIDManager::GetInstance();
        auto uid = m->GetVolumeID(step->GetPreStepPoint()->GetTouchable());
        att->FillUValue(uid);
      });
  DefineDigiAttribute(
      "PostStepUniqueVolumeID", 'U', FILLF {
        auto *m = GateUniqueVolumeIDManager::GetInstance();
        auto uid = m->GetVolumeID(step->GetPostStepPoint()->GetTouchable());
        att->FillUValue(uid);
      });
  DefineDigiAttribute(
      "HitUniqueVolumeID", 'U', FILLF {
        /*
          Like in old GATE (see GateCrystalSD.cc).
          However, no difference with PostStepUniqueVolumeID.
          Unsure if needed.
         */
        auto *m = GateUniqueVolumeIDManager::GetInstance();
        if (step->GetPostStepPoint()
                ->GetProcessDefinedStep()
                ->GetProcessName() == "Transportation") {
          auto uid = m->GetVolumeID(step->GetPreStepPoint()->GetTouchable());
          att->FillUValue(uid);
        } else {
          auto uid = m->GetVolumeID(step->GetPostStepPoint()->GetTouchable());
          att->FillUValue(uid);
        }
      });

  // -----------------------------------------------------
  // Position
  // FIXME -> add global/local position
  DefineDigiAttribute(
      "Position", '3',
      // Position is the same as PostPosition
      FILLF { att->Fill3Value(step->GetPostStepPoint()->GetPosition()); });
  DefineDigiAttribute(
      "PostPosition", '3',
      FILLF { att->Fill3Value(step->GetPostStepPoint()->GetPosition()); });
  DefineDigiAttribute(
      "PrePosition", '3',
      FILLF { att->Fill3Value(step->GetPreStepPoint()->GetPosition()); });
  DefineDigiAttribute(
      "EventPosition", '3', FILLFS {
        const auto *event = G4RunManager::GetRunManager()->GetCurrentEvent();
        auto p = event->GetPrimaryVertex(0)->GetPosition();
        att->Fill3Value(p);
      });
  DefineDigiAttribute(
      "TrackVertexPosition", '3',
      FILLF { att->Fill3Value(step->GetTrack()->GetVertexPosition()); });
  // -----------------------------------------------------
  // Direction
  DefineDigiAttribute(
      "Direction", '3',
      // Direction is the same as PostDirection
      FILLF {
        att->Fill3Value(step->GetPostStepPoint()->GetMomentumDirection());
      });
  DefineDigiAttribute(
      "PostDirection", '3', FILLF {
        att->Fill3Value(step->GetPostStepPoint()->GetMomentumDirection());
      });
  DefineDigiAttribute(
      "PreDirection", '3', FILLF {
        att->Fill3Value(step->GetPreStepPoint()->GetMomentumDirection());
      });
  DefineDigiAttribute(
      "PreDirectionLocal", '3', FILLF {
        const auto *theTouchable = step->GetPreStepPoint()->GetTouchable();
        auto dir = step->GetPreStepPoint()->GetMomentumDirection();
        dir = theTouchable->GetHistory()->GetTopTransform().TransformAxis(dir);
        att->Fill3Value(dir);
      });
  DefineDigiAttribute(
      "TrackVertexMomentumDirection", '3', FILLF {
        att->Fill3Value(step->GetTrack()->GetVertexMomentumDirection());
      });
  DefineDigiAttribute(
      "EventDirection", '3', FILLFS {
        const auto *event = G4RunManager::GetRunManager()->GetCurrentEvent();
        auto d =
            event->GetPrimaryVertex(0)->GetPrimary(0)->GetMomentumDirection();
        att->Fill3Value(d);
      });
}
